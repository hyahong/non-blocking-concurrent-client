#ifndef _CONNECTION_H_
# define _CONNECTION_H_

# include <iostream>
# include <cstring>
# include <fcntl.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <errno.h>
# include "TLSSocket.h"
# include "ClientRequest.h"
# include "ClientResponse.h"

class Cluster;

class Connection
{
public:
	/* coplien */
	Connection(Mode mode = Mode::BLOCKING);
	Connection(Connection const &conn);
	virtual ~Connection();

	Connection &operator=(Connection const &conn);

	/* exception */
	class InvalidNameServer : public std::exception {
		virtual const char *what() const throw();
	};

	class ConnectionFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class CTXAllocationFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class SSLAllocationFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class FailureToUseSNI : public std::exception {
		virtual const char *what() const throw();
	};

	class FailureToSetFD : public std::exception {
		virtual const char *what() const throw();
	};

	class ConnectSSLFailure : public std::exception {
		virtual const char *what() const throw();
	};

	class FailedToChangeFileFlag : public std::exception {
		virtual const char *what() const throw();
	};

	/* function */
	void SetCluster(Cluster &cluster);
	void SetMode(Mode mode);
	Cluster &GetCluster();

	void SetURL(std::string url);
	void SetMethod(Method method);

	void Connect();
	void Close();

	int GetSocket();
	Schema GetSchema();
	ClientRequest &GetRequest();
	ClientResponse &GetResponse();
	SSL *GetSSL();

	/* I/O */
	ssize_t write(const void *buf, size_t count);
	ssize_t read(void *buf, size_t count);

private:
	Cluster *_cluster;

	Mode _mode;

	int _socket;
	struct sockaddr_in _address;

	Schema _schema;

	TLSSocket _tls;

	ClientRequest _request;
	ClientResponse _response;

	/* function */
	void parseURL(std::string url);
	void parseDNS();

	void useTLS();
};

#endif
