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

const char *Cluster::NotSupportRangeRequest::what() const throw() {
	return ("Cluster: Server does not support range requests");
}

const char *Cluster::FailedToRequest::what() const throw() {
	return ("Cluster: Failed to request");
}

const char *Cluster::EpollCreateFailure::what() const throw() {
	return ("Cluster: Failed to create epoll file descriptor");
}

const char *Cluster::EpollWaitFailure::what() const throw() {
	return ("Cluster: Failed to wait epoll");
}

const char *Cluster::EpollCtlFailure::what() const throw() {
	return ("Cluster: Failed to control epoll file descriptor");
}

const char *Cluster::UndefinedSocket::what() const throw() {
	return ("Cluster: Undefined socket");
}

/* coplien */
Cluster::Cluster() :
	_fileblocksize(FILE_BLOCK_SIZE_DEFAULT),
	_workernumber(WORKER_NUMBER_DEFAULT),
	_name(""),
	_url(""),
	_path(""),
	_blocks(nullptr),
	_blockSize(0),
	_size(0),
	_stackedSize(0),
	_workers(nullptr),
	_workerSize(0)	
{
}

Cluster::Cluster(const Cluster &c)
{
	(void) c;
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
	(void) c;
	return *this;
}

/* public */
void Cluster::Download(std::string url, std::string path)
{
	Connection conn;

	/* clear */
	if (_blocks)
		delete[] _blocks;
	_blocks = nullptr;
	_blockSize = 0;
	_size = 0;
	_stackedSize = 0;
	if (_workers)
		delete[] _workers;
	_workers = nullptr;
	_workerSize = 0;

	/* init */
	if (url.compare(""))
		_url = url;
	if (path.compare(""))
		_path = path;
	
	/* get file info */
	requestHead(conn);
	if (conn.GetResponse().GetCode().compare("200"))
		throw FailedToRequest();
	_name = conn.GetRequest().GetPath().substr(1);
	/* does server support range reqeust */
	if (conn.GetResponse().GetHeader()["Accept-Ranges"].find("bytes") == std::string::npos)
		throw NotSupportRangeRequest();
	/* default path */
	if (!_path.compare(""))
		_path = conn.GetRequest().GetPath().substr(1);
	/* open */
	_file = ::open(_path.c_str(), O_WRONLY | O_CREAT, 0666);
	if (_file < 0)
		throw FileOpenFailure();
	_size = std::strtoull(conn.GetResponse().GetHeader()["Content-Length"].c_str(), NULL, 0);
	/* file to splited block */
	splitFileToBlocks();
	/* register tasks */
	useTaskQueue();
	/* non-blocking run */
	run();
}

void Cluster::Option(Convert &convert)
{
	if (convert._option.find("-w") != convert._option.end())
	{
		_workernumber = strtoul(convert._option["-w"].c_str(), NULL, 10);
	}
	if (convert._option.find("--worker") != convert._option.end())
	{
		_workernumber = strtoul(convert._option["--worker"].c_str(), NULL, 10);
	}

	if (convert._option.find("-d") != convert._option.end())
	{
		_path = convert._option["-d"];
	}
	if (convert._option.find("--destination") != convert._option.end())
	{
		_path = convert._option["--destination"];
	}

	if (convert._option.find("-s") != convert._option.end())
	{
		_fileblocksize = strtoull(convert._option["-s"].c_str(), NULL, 10);
	}
	if (convert._option.find("--size") != convert._option.end())
	{
		_fileblocksize = strtoull(convert._option["--size"].c_str(), NULL, 10);
	}

	_url = convert._argument[0];
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

	block = _size / _fileblocksize;
	if (_size % _fileblocksize > 0)
		++block;
	_blocks = new FileBlock[block];
	_blockSize = block;
	_blocks[0]._start = 0;
	for (unsigned long long int i = 0; i < block; ++i)
	{
		_blocks[i]._start = i * _fileblocksize;
		_blocks[i]._end = (i + 1) * _fileblocksize - 1;
	}
	_blocks[block - 1]._end = _size - 1;
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
	_workerSize = _workernumber > _blockSize ? _blockSize : _workernumber;
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
		_tasks.front()->_status = Status::RUNNING;
		_tasks.pop();
		_workers[i].Conn.GetRequest().SetRange(_workers[i].Info->GetStart(), _workers[i].Info->GetEnd());
		_workers[i].Conn.GetRequest().SetBuffer(_workers[i].Conn.GetRequest().GetStringHeader());
	}
}

bool Cluster::assign(worker_t *worker)
{
	if (_tasks.empty())
		return false;
	worker->Info = _tasks.front();
	_tasks.front()->_status = Status::RUNNING;
	_tasks.pop();
	worker->Conn.Reset();
	worker->Conn.GetRequest().SetRange(worker->Info->GetStart(), worker->Info->GetEnd());
	worker->Conn.GetRequest().SetBuffer(worker->Conn.GetRequest().GetStringHeader());
	return true;
}

void Cluster::failure(worker_t *worker)
{
	struct epoll_event event;

	/* fail */
	worker->Info->_status = Status::FAILED;
	/* remove from epoll */
	bzero(&event, sizeof(event));
	event.data.fd = worker->Conn.GetSocket();
	if (epoll_ctl(_epoll, EPOLL_CTL_DEL, event.data.fd, &event) < 0)
		throw EpollCtlFailure();
	/* re connection */
	worker->Conn.Close();
	worker->Conn.Connect();
	/* add failed block to queue */
	_tasks.push(worker->Info);
	/* new registration on epoll */
	bzero(&event, sizeof(event));
	event.data.fd = worker->Conn.GetSocket();
	event.events = EPOLLOUT | EPOLLERR | EPOLLHUP;
	if (epoll_ctl(_epoll, EPOLL_CTL_ADD, event.data.fd, &event) < 0)
		throw EpollCtlFailure();
	/* assign (can't fail) */
	assign(worker);
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

void Cluster::cycle(unsigned long long int &threshold)
{
	_stackedSize = 0;
	/* global */
	for (unsigned long long int i = 0; i < _blockSize; ++i)
		_stackedSize += _blocks[i].GetStackedSize();
	/* draw */
	if (_stackedSize > threshold)
	{
		print();
		threshold = _stackedSize + 1240 * 1024;
	}
}

void Cluster::print(bool bar)
{
	std::ostringstream str;
	unsigned long long int blockSize;
	int barPercent;

	/* system */
	system("clear");
	/* global */
	std::cout << _name << " ";
	std::cout << _stackedSize << "/" << _size << " (" << _stackedSize * 100 / _size << "%)" << std::endl << std::endl;
	/* local */
	for (unsigned long long int i = 0; i < _blockSize; i += PROGRESS_COLUMN)
	{
		for (unsigned long long int r = i; r < _blockSize && r < PROGRESS_COLUMN + i; ++r)
		{
			str.str("");
			str.clear();
			blockSize = _blocks[r]._end - _blocks[r]._start + 1;
			str << (_blocks[r]._isCached ? "(C) " : "");
			if (blockSize > 1024 * 1024 * 1024)
				str << _blocks[r]._stackedSize / 1024 / 1024 / 1024 << "/" << blockSize / 1024 / 1024 / 1024 << " GB ";
			else if (blockSize > 1024 * 1024)
				str << _blocks[r]._stackedSize / 1024 / 1024 << "/" << blockSize / 1024 / 1024 << " MB ";
			else if (blockSize > 1024)
				str << _blocks[r]._stackedSize / 1024 << "/" << blockSize / 1024 << " KB ";
			else
				str << _blocks[r]._stackedSize << "/" << blockSize << " B ";

			if (_blocks[r]._status == Status::WAITING)
				str << "WAITING";
			else if (_blocks[r]._status == Status::RUNNING)
				str << "RUNNING";
			else if (_blocks[r]._status == Status::DONE)
				str << "DONE";
			else if (_blocks[r]._status == Status::FAILED)
				str << "FAILED";

			std::cout << std::setw(PROGRESS_BAR_WIDTH) << str.str();
			std::cout << std::setw(PROGRESS_MARGIN) << "";
		}
		std::cout << std::endl;
		if (bar)
		{
			for (unsigned long long int r = i; r < _blockSize && r < PROGRESS_COLUMN + i; ++r)
			{
				blockSize = _blocks[r]._end - _blocks[r]._start + 1;
				barPercent = (int) (_blocks[r]._stackedSize * PROGRESS_BAR_WIDTH / blockSize);
				for (int j = 0; j < barPercent; ++j)
					std::cout << "█";
				for (int j = 0; j < PROGRESS_BAR_WIDTH - barPercent; ++j)
					std::cout << "░";
				std::cout << std::setw(PROGRESS_MARGIN) << "";
			}
			std::cout << std::endl;
		}
	}
}

/* socket I/O */
bool Cluster::epollRead(int socket)
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
	/* failure testcase
	if (!(bytes % 123))
	{
		failure(worker);
		return false;
	}
	*/
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
			failure(worker);
			return false;
		}
		else
			throw ReadFailure();
	}	
	/* receive */
	worker->Conn.GetResponse().Receive(buf, bytes, worker->Info->GetStart());
	worker->Info->SetStackedSize(worker->Conn.GetResponse().GetStackedSize());
	if (worker->Conn.GetResponse().IsHeaderCompleted())
	{
		if (worker->Conn.GetResponse().GetCode().compare("206"))
		{
			failure(worker); /* is valid type? */
			return false;
		}
		if (!worker->Conn.GetResponse().GetHeader()["X-Cache"].compare("Hit from cloudfront"))
			worker->Info->_isCached = true;
		return worker->Conn.GetResponse().IsBodyCompleted();
	}
	return false;
}

bool Cluster::epollWrite(int socket)
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
			failure(worker);
			return false;
		}
		else
			throw WriteFailure();
	}
	worker->Conn.GetRequest().Send(bytes);
	return worker->Conn.GetRequest().IsWriteDone();
}

void Cluster::readDone(int socket)
{
	struct epoll_event event;
	worker_t *worker;

	/* find worker */
	worker = findWorker(socket);
	if (!worker)
		throw UndefinedSocket();
	/* epoll */
	bzero(&event, sizeof(event));
	event.data.fd = socket;
	event.events = EPOLLOUT | EPOLLERR | EPOLLHUP;
	if (epoll_ctl(_epoll, EPOLL_CTL_MOD, event.data.fd, &event) < 0)
		throw EpollCtlFailure();
	/* set task */
	worker->Info->_status = Status::DONE;
	/* new */
	if (!assign(worker))
	{
		if (epoll_ctl(_epoll, EPOLL_CTL_DEL, event.data.fd, &event) < 0)
			throw EpollCtlFailure();
	}
}

void Cluster::writeDone(int socket)
{
	struct epoll_event event;

	bzero(&event, sizeof(event));
	event.data.fd = socket;
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	if (epoll_ctl(_epoll, EPOLL_CTL_MOD, event.data.fd, &event) < 0)
		throw EpollCtlFailure();
}

void Cluster::run()
{
	std::chrono::system_clock::time_point startTime, endTime;
	std::chrono::milliseconds diffMsec;
	unsigned long long int threshold;
	struct epoll_event *events;
	int ready;

	/* worker (connection) */
	makeWorker();

	/* allocate */
	events = new struct epoll_event[_workerSize + 1];
	bzero(events, (_workerSize + 1) * sizeof(struct epoll_event));

	/* epoll */
	_epoll = epoll_create(_workerSize);
    if (_epoll < 0)
		throw EpollCreateFailure();
	for (unsigned int i = 0; i < _workerSize; ++i)
	{
		struct epoll_event event;

		bzero(&event, sizeof(event));
		event.data.fd = _workers[i].Conn.GetSocket();
		event.events = EPOLLOUT | EPOLLERR | EPOLLHUP;

		if (epoll_ctl(_epoll, EPOLL_CTL_ADD, event.data.fd, &event) < 0)
			throw EpollCtlFailure();
	}
	/* start */
	threshold = 1;
    startTime = std::chrono::system_clock::now();
	/* non-blocking I/O */
	while (_stackedSize != _size)
	{
		ready = epoll_wait(_epoll, events, _workerSize, 100);
		if (ready < 0)
		{
			if (ready == EINTR)
				continue;
			throw EpollWaitFailure();
		}
		/* handles error, read, write in order */
		for (int i = 0; i < ready; ++i)
		{
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & (EPOLLIN | EPOLLOUT))))
				continue;
			else if (events->events & EPOLLIN && (epollRead(events[i].data.fd)))
				readDone(events[i].data.fd);
			else if (events->events & EPOLLOUT && epollWrite(events[i].data.fd))
				writeDone(events[i].data.fd);
			cycle(threshold);
		}
	}

	print();
    endTime = std::chrono::system_clock::now();
	diffMsec = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << "time: " << (double) diffMsec.count() / 1000 << " s" << std::endl;
	delete[] events;
}
