#ifndef _CONVERT_H_
# define _CONVERT_H_

# include <map>
# include <vector>
# include <iostream>
# include <cstring>

class Convert
{
	friend class Cluster;

public:
	/* coplien */
	Convert();
	Convert(const Convert &convert);
	virtual ~Convert();

	Convert &operator=(const Convert &convert);

	/* function */
	bool Parser(int argc, char *argv[]);

private:
	std::map<std::string, std::string> _option;
	std::vector<std::string> _argument;

	/* function */
	void help();
};

#endif
