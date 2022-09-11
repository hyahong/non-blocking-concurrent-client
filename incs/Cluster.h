#ifndef _CLUSTER_H_
# define _CLUSTER_H_

# include <sstream>
# include <iomanip>
# include <queue>
# include <fcntl.h>
# include <sys/epoll.h>
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

	class WriteFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class ReadFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class FailedToRequest : public std::exception {
		virtual const char *what() const throw();
	};

	class EpollCreateFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class EpollCtlFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class UndefinedSocket : public std::exception {
		virtual const char *what() const throw();
	};

	/* function */
	void Download(std::string url, std::string path);

	/* I/O */
	void flush(const char *buf, size_t count, off_t offset);

private:
	/* struct */
	struct _worker
	{
		/* public */
		FileBlock *Info;
		Connection Conn;
	};
	typedef _worker worker_t;

	/* member */
	int _file;

	std::string _url;
	std::string _path;

	FileBlock *_blocks;
	unsigned long long int _blockSize;

	unsigned long long int _size;
	unsigned long long int _stackedSize;

	std::queue<FileBlock *> _tasks;

	worker_t *_workers;
	unsigned int _workerSize;

	/* function */
	void requestHead(Connection &conn);
	void splitFileToBlocks();
	void useTaskQueue();
	void makeWorker();

	worker_t *findWorker(int socket);

	void cycle(int &callcycle);
	void print(bool bar = true);

	/* non-blocking */
	bool epollRead(int socket);
	bool epollWrite(int socket);
	void readDone(int epollFd, int socket);
	void writeDone(int epollFd, int socket);

	void run();
};

#endif
