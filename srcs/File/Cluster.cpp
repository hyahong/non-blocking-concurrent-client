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
	/* worker (connection) */
	makeWorker();
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
		conn.GetResponse().Receive(buf, bytes);
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

void Cluster::makeWorker()
{
	_workerSize = WORKER_NUMBER > _blockSize ? _blockSize : WORKER_NUMBER;
	_workers = new worker_t[_workerSize];
	for (unsigned int i = 0; i < _workerSize; ++i)
	{
		_workers[i].Info = &_blocks[i];
		_workers[i].Conn.SetCluster(*this);
		_workers[i].Conn.SetMethod(Method::GET);
		_workers[i].Conn.SetURL(_url);
		_workers[i].Conn.Connect();
	}
}

void Cluster::run()
{
}
