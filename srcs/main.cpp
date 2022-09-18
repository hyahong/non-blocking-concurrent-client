#include <iostream>
#include "Cluster.h"

int		main(void)
{
	Cluster cluster;

	try {
		cluster.Download("http://d1f0m5hlxy4kyb.cloudfront.net/debian-11.4.0-amd64-netinst.iso", "target.iso");
		//cluster.Download("https://originbucket225.s3.ap-northeast-2.amazonaws.com/debian-11.4.0-amd64-netinst.iso", "target.iso");
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
