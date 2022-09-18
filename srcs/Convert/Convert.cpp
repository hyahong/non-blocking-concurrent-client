#include "Convert.h"

/* coplien */
Convert::Convert()
{
}

Convert::Convert(const Convert &convert)
{
	(void) convert;
}

Convert::~Convert()
{
}

Convert &Convert::operator=(const Convert &convert)
{
	(void) convert;

	return *this;
}

/* public */
bool Convert::Parser(int argc, char *argv[])
{
	const char *flags[] = { "-w", "--worker", "-d", "--destination", "-s", "--size" };
	char *argument;
	bool isValid;

	argument = NULL;
	_option.clear();
	_argument.clear();
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
		{
			/* help */
			help();
			return false;
		}
		if (!strncmp(argv[i], "-", 1) || !strncmp(argv[i], "--", 2))
		{
			isValid = false;
			for (int j = 0; j < 3 * 2; ++j)
			{
				if (!strcmp(argv[i], flags[j]))
				{
					isValid = true;
					break;
				}
			}
			if (!isValid)
			{
				/* invalid option */
				std::cout << "invalid option " << argv[i] << std::endl;
				help();
				return false;
			}
			if (i + 1 >= argc || !strncmp(argv[i + 1], "-", 1) || !strncmp(argv[i + 1], "--", 2))
			{
				/* missing field */
				std::cout << "option requires an argument " << argv[i] << std::endl;
				help();
				return false;
			}
			std::string first(argv[i]), second(argv[i + 1]);
			_option.insert({ first, second });
			++i;
		}
		else
			argument = argv[i];
	}
	if (!argument)
	{
		/* missing argument */
		std::cout << "argument required" << std::endl;
		help();
		return false;
	}
	std::string value(argument);
	_argument.push_back(value);
	return true;
}

/* private */
void Convert::help()
{
	std::cout << std::endl;
	std::cout << "SYNOPSIS" << std::endl;
	std::cout << "      ./client [OPTIONS]... [SOURCE]" << std::endl << std::endl;

	std::cout << "DESCRIPTION" << std::endl;

	std::cout << "      -w, --worker WORKER" << std::endl;
	std::cout << "         specifies the number of workers. default is 10." << std::endl << std::endl;
	
	std::cout << "      -d, --destination DEST" << std::endl;
	std::cout << "         specifies the location where the file will be saved." << std::endl << std::endl;

	std::cout << "      -s, --size SIZE" << std::endl;
	std::cout << "         specifies the size of one file block. default is 20971520." << std::endl << std::endl;
}
