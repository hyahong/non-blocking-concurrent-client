#include <iostream>
#include "Connection.h"

int		main(void)
{
	Connection conn;
	std::string header;
	char buffer[2048];

	try {
		conn.SetMethod(Method::HEAD);
		conn.SetURL("https://d1f0m5hlxy4kyb.cloudfront.net/kali-linux-2019.2-amd64.iso");
		conn.Connect();
		header = conn.GetRequest().GetStringHeader();
		std::cout << header << std::endl;
		conn.write(header.c_str(), header.length());
		int bytes = conn.read(buffer, 2047);
		conn.GetResponse().Receive(buffer, bytes);
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
