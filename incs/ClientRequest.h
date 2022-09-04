#ifndef _CLIENTREQUEST_H_
# define _CLIENTREQUEST_H_

# include <map>
# include <string>
# include "type.h"

class ClientRequest
{
	friend class Connection;

public:
	ClientRequest();
	ClientRequest(ClientRequest const &req);
	virtual ~ClientRequest();

	ClientRequest &operator=(ClientRequest const &req);

	std::map<std::string, std::string> &GetHeader();

private:
	Method _method;
	std::string _path;
	std::string _version;

	std::map<std::string, std::string> _header;
};

#endif
