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
	char buf[20];
	int bytes;

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
