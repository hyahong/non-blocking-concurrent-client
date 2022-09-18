#include <iostream>
#include "Convert.h"
#include "Cluster.h"

int		main(int argc, char *argv[])
{
	Convert convert;
	Cluster cluster;

	try {
		if (!convert.Parser(argc, argv))
			return 0;
		cluster.Option(convert);
		cluster.Download();
	}
	catch (std::exception &e) {
		std::cout << "error occurs at: ";
		std::cout << e.what() << std::endl;
	}
	return 0;
}
