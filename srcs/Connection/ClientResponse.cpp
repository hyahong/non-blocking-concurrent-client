#include "ClientResponse.h"

/* coplien */
ClientResponse::ClientResponse() :
	_flag(false),
	_buffer(nullptr),
	_bufferSize(0),
	_offset(0),
	_stackedOffset(0),
	_size(0),
	_version(""),
	_code(""),
	_status("")
{
	_bufferSize = 1;
	_buffer = new char[_bufferSize + 1];
}

ClientResponse::ClientResponse(ClientResponse const &res)
{
}

ClientResponse::~ClientResponse()
{
	if (_buffer)
		delete[] _buffer;
}

ClientResponse &ClientResponse::operator=(ClientResponse const &res)
{
	return *this;
}

/* public */
void ClientResponse::Receive(char *buf, size_t count)
{
	char *pos;

	if (_offset + count >= _bufferSize)
		expandBuffer(_offset + count);
	for (int i = 0; i < count; i++)
		_buffer[_offset + i] = buf[i];
	_offset += count;
	_buffer[_offset] = 0;

	if (_flag)
	{
		_stackedOffset += count;
		/* body */
		if (_offset > 4)
		{
//			std::cout << "{" << _buffer << "}";
		//	_connection->GetCluster().flush();
			_offset = 0;
			_buffer[_offset] = 0;
		}
		if (IsBodyCompleted() && _offset > 0)
		{
//			std::cout << "{" << _buffer << "}";
			_offset = 0;
			_buffer[_offset] = 0;
		}
	}
	if (!_flag && isHeaderCompleted())
	{
		/* header */
		pos = strstr(_buffer, "\r\n\r\n");
		parseHeader(pos - _buffer + 2);
		for (int i = 0; i < _offset - (pos - _buffer + 4); i++)
			_buffer[i] = _buffer[pos - _buffer + 4 + i];
		_offset = _offset - (pos - _buffer + 4);
		_stackedOffset = _offset;
		_buffer[_offset] = 0;
		_size = std::strtoull(_header["Content-Length"].c_str(), NULL, 10);
		_flag = true;
	}
}

void ClientResponse::SetConnection(Connection &conn)
{
	_connection = &conn;
}

bool ClientResponse::IsHeaderCompleted()
{
	return _flag;
}

bool ClientResponse::IsBodyCompleted()
{
	return _flag && _stackedOffset == _size;
}

std::string ClientResponse::GetVersion()
{
	return _version;
}

std::string ClientResponse::GetCode()
{
	return _code;
}

std::string ClientResponse::GetStatus()
{
	return _status;
}

char *ClientResponse::GetBody()
{
	if (!_flag)
		return NULL;
	return _buffer;
}

unsigned long long int ClientResponse::GetBodySize()
{
	return _size;
}

std::map<std::string, std::string> &ClientResponse::GetHeader()
{
	return _header;
}

/* private */
void ClientResponse::expandBuffer(unsigned long long int offset)
{
	unsigned long long int newSize = _bufferSize;
	char *newBuffer;

	while (!(newSize > offset))
		newSize *= 2;
	newBuffer = new char[newSize + 1];
	for (int i = 0; i < _offset; i++)
		newBuffer[i] = _buffer[i];
	delete[] _buffer;
	_buffer = newBuffer;
	_bufferSize = newSize;
}

bool ClientResponse::isHeaderCompleted()
{
	if (strstr(_buffer, "\r\n\r\n"))
		return true;
	return false;
}

void ClientResponse::parseHeader(size_t size)
{
	unsigned int line;
	size_t offset;
	size_t cursor;

	line = 0;
	offset = 0;
	while (offset < size)
	{
		for (cursor = offset; cursor < size && _buffer[cursor] != '\r'; ++cursor);
		_buffer[cursor] = 0;
		if (line == 0)
			parseStatus(_buffer + offset);
		else
			parsePairHeader(_buffer + offset);
		offset += cursor - offset + 2;
		++line;
	}
}

void ClientResponse::parseStatus(char *line)
{
	line[8] = line[12] = 0;
	_version = line;
	_code = line + 9;
	_status = line + 13;
}

void ClientResponse::parsePairHeader(char *line)
{
	char *second;

	for (int i = 0; line[i]; ++i)
	{
		if (line[i] == ':')
		{
			line[i] = 0;
			second = &line[i + 2];
			break;
		}
	}
	_header.insert({ line, second });
}
