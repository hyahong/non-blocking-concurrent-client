#include "Cluster.h"

/* coplien */
Cluster::Cluster()
{
}

Cluster::Cluster(const Cluster &c)
{
}

Cluster::~Cluster()
{
}

Cluster &Cluster::operator=(const Cluster &c)
{
	return *this;
}

void Cluster::Download(std::string url, std::string path)
{
	Connection conn;
	std::string header;
	char buf[3];
	int bytes;

	/* head */
	conn.SetMethod(Method::GET);
	conn.SetURL(url);
	/* connect */
	conn.Connect();

	std::cout << "?" << std::endl;
	/* request */
	conn.GetRequest().GetHeader()["Range"] = "bytes: 1-20";
	std::cout << conn.GetRequest().GetOffset() << std::endl;

	conn.GetRequest().SetBuffer(conn.GetRequest().GetStringHeader());
	while (!conn.GetRequest().IsWriteDone())
	{
		bytes = conn.write(conn.GetRequest().GetOffset(), conn.GetRequest().GetRemainder());
		conn.GetRequest().Send(bytes);
	}

	/* response */
	while (!conn.GetResponse().IsHeaderCompleted())
	{
		bytes = conn.read(buf, sizeof(buf) - 1);
		std::cout << buf << std::endl;
		conn.GetResponse().Receive(buf, bytes);
	}

	std::cout << conn.GetResponse().GetCode() << std::endl;

	conn.Close();
}
