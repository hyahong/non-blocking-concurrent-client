#ifndef _CLUSTER_H_
# define _CLUSTER_H_

# include <fcntl.h>
# include "Connection.h"
# include "FileBlock.h"

class Cluster
{
public:
	/* coplien */
	Cluster();
	Cluster(const Cluster &c);
	virtual ~Cluster();

	Cluster &operator=(const Cluster &c);

	/* exception */
	class FileOpenFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class FailedToSetOffset : public std::exception {
		virtual const char *what() const throw();
	};

	/* function */
	void Download(std::string url, std::string path);

	/* I/O */
	void flush(const char *buf, size_t count, off_t offset);

private:
	int _file;
};

#endif
