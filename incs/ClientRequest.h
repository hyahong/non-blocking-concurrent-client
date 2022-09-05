#ifndef _CLIENTREQUEST_H_
# define _CLIENTREQUEST_H_

# include <map>
# include <string>
# include "type.h"

class ClientRequest
{
	friend class Connection;

public:
	/* coplien */
	ClientRequest();
	ClientRequest(ClientRequest const &req);
	virtual ~ClientRequest();

	ClientRequest &operator=(ClientRequest const &req);

	/* function */
	std::map<std::string, std::string> &GetHeader();
	std::string GetStringHeader();

private:
	Method _method;
	std::string _path;
	std::string _version;

	std::map<std::string, std::string> _header;

	char *_buffer;

	unsigned long long int _offset;
	unsigned long long int _size;

	/* function */
	std::string methodToString(Method m);
};

#endif
