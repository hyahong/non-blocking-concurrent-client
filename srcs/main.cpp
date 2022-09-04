#include <iostream>
#include "Connection.h"

int		main(void)
{
	Connection conn;

	conn.SetMethod(Method::GET);

	conn.SetURL("https://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
	conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
	conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net/");
	conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net");
	std::cout << conn.SetURL("cloudfront.net") << std::endl;
	//conn.Connect();
	return 0;
}
