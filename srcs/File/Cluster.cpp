#include "Cluster.h"

/* exception */
const char *Cluster::FileOpenFailure::what() const throw() {
	return ("Cluster: Failed to open a file");
}

const char *Cluster::FailedToSetOffset::what() const throw() {
	return ("Cluster: Failed to set the offset");
}

/* coplien */
Cluster::Cluster()
{
}

Cluster::Cluster(const Cluster &c)
{
}

Cluster::~Cluster()
{
	::close(_file);
}

Cluster &Cluster::operator=(const Cluster &c)
{
	return *this;
}

/* public */
void Cluster::Download(std::string url, std::string path)
{
	_file = ::open(path.c_str(), O_WRONLY | O_CREAT, 0666);
	if (_file < 0)
		throw FileOpenFailure();

	Connection conn;
	std::string header;
	char buf[1024];
	ssize_t bytes;

	conn.SetCluster(*this);

	/* head */
	conn.SetMethod(Method::GET);
	conn.SetURL(url);
	/* connect */
	conn.Connect();

	/* request */
	conn.GetRequest().GetHeader()["Range"] = "bytes=0-30";
	
	conn.GetRequest().SetBuffer(conn.GetRequest().GetStringHeader());
	while (!conn.GetRequest().IsWriteDone())
	{
		bytes = conn.write(conn.GetRequest().GetOffset(), conn.GetRequest().GetRemainder());
		conn.GetRequest().Send(bytes);
	}

	/* response */
	while (!conn.GetResponse().IsHeaderCompleted() || !conn.GetResponse().IsBodyCompleted())
	{
		bytes = conn.read(buf, sizeof(buf) - 1);
		conn.GetResponse().Receive(buf, bytes);
	}

//	std::cout << conn.GetResponse().GetHeader()["Content-Length"] << std::endl;
//	std::cout << conn.GetResponse().GetHeader()["Content-Range"] << std::endl;
	std::cout << conn.GetResponse().GetCode() << std::endl;
	std::cout << conn.GetResponse().GetBody() << std::endl;

	conn.Close();
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
