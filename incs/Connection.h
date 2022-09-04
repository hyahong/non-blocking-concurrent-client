#ifndef _CONNECTION_H_
# define _CONNECTION_H_

# include <iostream>
# include <cstring>
# include "TLSSocket.h"
# include "ClientRequest.h"
# include "ClientResponse.h"
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

class Connection
{
public:
	/* coplien */
	Connection();
	Connection(Connection const &conn);
	virtual ~Connection();

	Connection &operator=(Connection const &conn);

	/* exception */
	class InvalidNameServer : public std::exception
	{
		virtual const char *what() const throw();
	};

	/* function */
	void SetURL(std::string url);
	void SetMethod(Method method);
	bool Connect();

private:
	int _socket;
	struct sockaddr_in _address;

	Schema _schema;

	TLSSocket _tls;

	ClientRequest _request;
	ClientResponse _response;

	/* function */
	void parseURL(std::string url);
	void parseDNS();
};

#endif
