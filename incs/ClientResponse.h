#ifndef _CLIENTRESPONSE_H_
# define _CLIENTRESPONSE_H_

# include <map>
# include <string>
# include <cstring>
# include <cstdlib>
# include "type.h"

class Connection;

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
	void Receive(char *buf, size_t count, unsigned long long int offset);

	void SetConnection(Connection &conn);

	bool IsHeaderCompleted();
	bool IsBodyCompleted();

	std::string GetVersion();
	std::string GetCode();
	std::string GetStatus();

	char *GetBody();
	unsigned long long int GetBodySize();

	std::map<std::string, std::string> &GetHeader();

private:
	Connection *_connection;

	bool _flag;

	char *_buffer;
	unsigned long long int _bufferSize;

	unsigned long long int _offset;
	unsigned long long int _fileOffset;
	unsigned long long int _stackedOffset;
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
