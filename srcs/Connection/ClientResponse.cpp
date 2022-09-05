#include "ClientResponse.h"
#include <iostream>

/* coplien */
ClientResponse::ClientResponse() :
	_flag(false),
	_buffer(nullptr),
	_bufferSize(0),
	_offset(0),
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
	if (_offset + count >= _bufferSize)
		expandBuffer(_offset + count);
	for (int i = 0; i < count; i++)
		_buffer[_offset + i] = buf[i];
	_offset += count;
	_buffer[_offset] = 0;

	if (!_flag && isHeaderCompleted())
	{
		parseHeader(strstr(_buffer, "\r\n\r\n") - _buffer + 2);
		for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		{
			std::cout << it->first << " : " << it->second << std::endl;
		}
		std::cout << _header.size() << std::endl;
		//remainder to body
		_flag = true;
	}
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
	std::cout << line << std::endl;
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
	std::cout << line <<":"<< second << std::endl;
	_header.insert({ line, second });
}
