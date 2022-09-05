#include <iostream>
#include "Connection.h"

int		main(void)
{
	Connection conn;

	try {
		conn.SetMethod(Method::GET);
		conn.SetURL("https://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
		conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
		conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net/");
		conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net");
		//conn.SetURL("cloudfront.net");
		conn.Connect();
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
