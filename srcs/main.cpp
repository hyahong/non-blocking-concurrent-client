#include <iostream>
#include "Connection.h"

int		main(void)
{
	Connection conn;
	std::string header;

	try {
		conn.SetMethod(Method::GET);
		conn.SetURL("https://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
		conn.Connect();
		header = conn.GetReqeustStringHeader();
		conn.write(header.c_str(), header.length());
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
