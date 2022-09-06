#ifndef _CLIENTRESPONSE_H_
# define _CLIENTRESPONSE_H_

# include <map>
# include <string>
# include <cstring>
# include "type.h"

class ClientResponse
{
	friend class Connection;

public:
	/* coplien */
	ClientResponse();
	ClientResponse(ClientResponse const &res);
	virtual ~ClientResponse();

	ClientResponse &operator=(ClientResponse const &res);

	/* function */
	void Receive(char *buf, size_t count);
	bool IsHeaderCompleted();

	std::string GetVersion();
	std::string GetCode();
	std::string GetStatus();

	std::map<std::string, std::string> &GetHeader();

private:
	bool _flag;

	char *_buffer;
	unsigned long long int _bufferSize;

	unsigned long long int _offset;
	unsigned long long int _size;

	/* header */
	std::string _version;
	std::string _code;
	std::string _status;
	std::map<std::string, std::string> _header;

	/* function */
	void expandBuffer(unsigned long long int offset);
	bool isHeaderCompleted();

	void parseHeader(size_t size);
	void parseStatus(char *line);
	void parsePairHeader(char *line);
};

#endif
