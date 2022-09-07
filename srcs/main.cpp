#include <iostream>
#include "Cluster.h"

int		main(void)
{
	Cluster cluster;

	try {
		//cluster.Download("https://d1f0m5hlxy4kyb.cloudfront.net/test.txt", "target.txt");
		cluster.Download("https://d1f0m5hlxy4kyb.cloudfront.net/kali-linux-2019.2-amd64.iso", "target.iso");
		/*
		conn.SetMethod(Method::HEAD);
		//conn.SetURL("https://d1f0m5hlxy4kyb.cloudfront.net/kali-linux-2019.2-amd64.iso");
		conn.SetURL("http://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
		conn.Connect();
		header = conn.GetRequest().GetStringHeader();
		std::cout << header << std::endl;
		conn.write(header.c_str(), header.length());
		int bytes = conn.read(buffer, 2047);
		conn.GetResponse().Receive(buffer, bytes);
		std::cout << buffer << std::endl;
		*/
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
