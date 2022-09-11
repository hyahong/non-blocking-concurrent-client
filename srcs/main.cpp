#include <iostream>
#include "Cluster.h"

int		main(void)
{
	Cluster cluster;

	try {
		cluster.Download("http://d1f0m5hlxy4kyb.cloudfront.net/test.txt", "target.txt");
		//cluster.Download("http://d1f0m5hlxy4kyb.cloudfront.net/kali-linux-2019.2-amd64.iso", "target.iso");
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
