#ifndef _CONNECTION_H_
# define _CONNECTION_H_

# include <iostream>
# include <cstring>
# include "TLSSocket.h"
# include "ClientRequest.h"
# include "ClientResponse.h"
# include <netinet/in.h>

class Connection
{
public:
	Connection();
	Connection(Connection const &conn);
	virtual ~Connection();

	Connection &operator=(Connection const &conn);

	bool SetURL(std::string url);
	void SetMethod(Method method);
	bool Connect();

private:
	int _socket;
	struct sockaddr_in _address;

	Schema _schema;

	TLSSocket _tls;

	ClientRequest _request;
	ClientResponse _response;

	bool parseURL(std::string url);
};

#endif
