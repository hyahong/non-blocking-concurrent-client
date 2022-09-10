#include "Cluster.h"

/* exception */
const char *Cluster::FileOpenFailure::what() const throw() {
	return ("Cluster: Failed to open a file");
}

const char *Cluster::FailedToSetOffset::what() const throw() {
	return ("Cluster: Failed to set the offset");
}

const char *Cluster::WriteFailure::what() const throw() {
	return ("Cluster: write failed");
}

const char *Cluster::ReadFailure::what() const throw() {
	return ("Cluster: read failed");
}

const char *Cluster::FailedToRequest::what() const throw() {
	return ("Cluster: Failed to request");
}

const char *Cluster::EpollCreateFailure::what() const throw() {
	return ("Cluster: Failed to create epoll file descriptor");
}

const char *Cluster::EpollCtlFailure::what() const throw() {
	return ("Cluster: Failed to control epoll file descriptor");
}

const char *Cluster::UndefinedSocket::what() const throw() {
	return ("Cluster: Undefined socket");
}

/* coplien */
Cluster::Cluster() :
	_url(""),
	_path(""),
	_blocks(nullptr),
	_blockSize(0),
	_size(0),
	_workers(nullptr),
	_workerSize(0),
	_blockOffset(0)
{
}

Cluster::Cluster(const Cluster &c)
{
}

Cluster::~Cluster()
{
	::close(_file);
	if (_blocks)
		delete[] _blocks;
	for (unsigned int i = 0; i < _workerSize; ++i)
		_workers[i].Conn.Close();
	if (_workers)
		delete[] _workers;
}

Cluster &Cluster::operator=(const Cluster &c)
{
	return *this;
}

/* public */
void Cluster::Download(std::string url, std::string path)
{
	Connection conn;

	_url = url;
	_path = path;

	/* open */
	_file = ::open(path.c_str(), O_WRONLY | O_CREAT, 0666);
	if (_file < 0)
		throw FileOpenFailure();
	/* get file info */
	requestHead(conn);
	if (conn.GetResponse().GetCode().compare("200"))
		throw FailedToRequest();
// dose file server support range request?
	_size = std::strtoull(conn.GetResponse().GetHeader()["Content-Length"].c_str(), NULL, 0);
	/* file to splited block */
	splitFileToBlocks();
	/* register tasks */
	useTaskQueue();
	/* non-blocking run */
	run();
}

/* I/O */
void Cluster::flush(const char *buf, size_t count, off_t offset)
{
	size_t written;
	ssize_t bytes;

	if (lseek(_file, offset, SEEK_SET) < 0)
		throw FailedToSetOffset();

	written = 0;
	while (written < count)
	{
		bytes = write(_file, buf, count);
		written += bytes;
	}
}

/* private */
void Cluster::requestHead(Connection &conn)
{
	char buf[RECEIVE_BUFFER_SIZE];
	ssize_t bytes;

	conn.SetCluster(*this);
	/* head */
	conn.SetMethod(Method::HEAD);
	conn.SetURL(_url);
	/* connect */
	conn.Connect();
	/* request */
	conn.GetRequest().SetBuffer(conn.GetRequest().GetStringHeader());
	while (!conn.GetRequest().IsWriteDone())
	{
		bytes = conn.write(conn.GetRequest().GetOffset(), conn.GetRequest().GetRemainder());
		if (bytes < 0)
			throw WriteFailure();
		conn.GetRequest().Send(bytes);
	}
	/* response */
	while (!conn.GetResponse().IsHeaderCompleted())
	{
		bytes = conn.read(buf, sizeof(buf) - 1);
		if (bytes < 0)
			throw ReadFailure();
		conn.GetResponse().Receive(buf, bytes, 0);
	}
	/* close */
	conn.Close();
}

void Cluster::splitFileToBlocks()
{
	unsigned long long int block;

	block = _size / FILE_BLOCK_SIZE;
	if (_size % FILE_BLOCK_SIZE > 0)
		++block;
	_blocks = new FileBlock[block];
	_blockSize = block;
	_blocks[0]._start = 0;
	for (unsigned long long int i = 0; i < block; ++i)
	{
		_blocks[i]._start = i * FILE_BLOCK_SIZE;
		_blocks[i]._end = (i + 1) * FILE_BLOCK_SIZE - 1;
	}
	_blocks[block - 1]._end = _size;
}

void Cluster::useTaskQueue()
{
	/* clear */
	std::queue<FileBlock *> emptyQueue;
	std::swap(_tasks, emptyQueue);
	/* enqueue */
	for (unsigned long long int i = 0; i < _blockSize; ++i)
	{
		_tasks.push(&_blocks[i]);
	}
}

void Cluster::makeWorker()
{
	_workerSize = WORKER_NUMBER > _blockSize ? _blockSize : WORKER_NUMBER;
	_workers = new worker_t[_workerSize];
	for (unsigned int i = 0; i < _workerSize; ++i)
	{
		_workers[i].Conn.SetMode(Mode::NONBLOCKING);
		_workers[i].Conn.SetCluster(*this);
		_workers[i].Conn.SetMethod(Method::GET);
		_workers[i].Conn.SetURL(_url);
		_workers[i].Conn.Connect();
		/* init setting */
		_workers[i].Info = _tasks.front();
		_tasks.pop();
		_workers[i].Conn.GetRequest().SetRange(_workers[i].Info->GetStart(), _workers[i].Info->GetEnd());
		_workers[i].Conn.GetRequest().SetBuffer(_workers[i].Conn.GetRequest().GetStringHeader());
	}
}

Cluster::worker_t *Cluster::findWorker(int socket)
{
	for (unsigned int i = 0; i < _workerSize; ++i)
	{
		if (_workers[i].Conn.GetSocket() == socket)
			return &_workers[i];
	}
	return NULL;
}

/* socket I/O */
bool Cluster::epollRead(int epollFd, int socket)
{
	worker_t *worker;
	char buf[RECEIVE_BUFFER_SIZE + 1];
	int bytes;
	int error;

	worker = findWorker(socket);
	if (!worker)
		throw UndefinedSocket();
	/* read */
	bytes = worker->Conn.read(buf, sizeof(buf) - 1);
	if (bytes <= 0)
	{
		if (worker->Conn.GetSchema() == Schema::HTTP)
			return false;

		/* https */
		error = SSL_get_error(worker->Conn.GetSSL(), bytes);
		if(error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE || error == SSL_ERROR_WANT_X509_LOOKUP)
			return false;
		else if(error == SSL_ERROR_ZERO_RETURN)
		{
			/* disconnected by server */
			return false;
		}
		else
			throw ReadFailure();
	}
	worker->Conn.GetResponse().Receive(buf, bytes, worker->Info->GetStart());
	if (worker->Conn.GetResponse().IsHeaderCompleted() && worker->Conn.GetResponse().IsBodyCompleted())
		return true;
	return false;
}

bool Cluster::epollWrite(int epollFd, int socket)
{
	worker_t *worker;
	int bytes;
	int error;

	worker = findWorker(socket);
	if (!worker)
		throw UndefinedSocket();
	/* write */
	bytes = worker->Conn.write(worker->Conn.GetRequest().GetOffset(), worker->Conn.GetRequest().GetRemainder());
	if (bytes <= 0)
	{
		if (worker->Conn.GetSchema() == Schema::HTTP)
			return false;

		/* https */
		error = SSL_get_error(worker->Conn.GetSSL(), bytes);
		if(error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE || error == SSL_ERROR_WANT_X509_LOOKUP)
			return false;
		else if(error == SSL_ERROR_ZERO_RETURN)
		{
			/* disconnected by server */
			return false;
		}
		else
			throw ReadFailure();
	}
	worker->Conn.GetRequest().Send(bytes);
	if (worker->Conn.GetRequest().IsWriteDone())
		return true;
	return false;
}

void Cluster::readDone(int epollFd, int socket)
{
	struct epoll_event event;
	worker_t *worker;

	/* find worker */
	worker = findWorker(socket);
	if (!worker)
		throw UndefinedSocket();
	/* epoll */
	event.data.fd = socket;
	event.events = EPOLLOUT | EPOLLERR | EPOLLHUP;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, event.data.fd, &event) < 0)
	{
		perror("Epoll ctl: ");
		throw EpollCtlFailure();
	}
	/* set task */
	if (_tasks.empty())
	{
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, event.data.fd, &event) < 0)
		{
			perror("Epoll ctl: ");
			throw EpollCtlFailure();
		}
		//std::cout << "stop worker" << std::endl;
		return ;
	}
	worker->Info = _tasks.front();
	_tasks.pop();
	worker->Conn.GetResponse().Reset();
	worker->Conn.GetRequest().SetRange(worker->Info->GetStart(), worker->Info->GetEnd());
	worker->Conn.GetRequest().SetBuffer(worker->Conn.GetRequest().GetStringHeader());
}

void Cluster::writeDone(int epollFd, int socket)
{
	struct epoll_event event;

	event.data.fd = socket;
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, event.data.fd, &event) < 0)
	{
		perror("Epoll ctl: ");
		throw EpollCtlFailure();
	}
}

void Cluster::run()
{
	struct epoll_event *events;
	int epollFd;
	int ready;

	/* worker (connection) */
	makeWorker();

	/* allocate */
	events = new struct epoll_event[_workerSize + 1];
	bzero(events, (_workerSize + 1) * sizeof(struct epoll_event));

	/* epoll */
	epollFd = epoll_create(_workerSize);
    if (epollFd < 0)
	{
        perror("Epoll create: ");
		throw EpollCreateFailure();
    }
	for (unsigned int i = 0; i < _workerSize; ++i)
	{
		struct epoll_event event;

		event.data.fd = _workers[i].Conn.GetSocket();
		event.events = EPOLLOUT | EPOLLERR | EPOLLHUP;

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event) < 0)
		{
			perror("Epoll ctl: ");
			throw EpollCtlFailure();
		}
	}

	/* non-blocking I/O */
	while (1)
	{
		ready = epoll_wait(epollFd, events, _workerSize, 100);
		if (ready < 0 && ready == EINTR)
			continue;
		/* handles error, read, write in order */
		for (int i = 0; i < ready; ++i)
		{
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & (EPOLLIN | EPOLLOUT))))
				continue;
			else if (events->events & (EPOLLIN | EPOLLHUP) && epollRead(epollFd, events[i].data.fd))
				readDone(epollFd, events[i].data.fd);
			else if (events->events & EPOLLOUT && epollWrite(epollFd, events[i].data.fd))
				writeDone(epollFd, events[i].data.fd);
		}
	}

	delete[] events;
}
