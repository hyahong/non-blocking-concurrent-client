#include "ClientRequest.h"

/* coplien */
ClientRequest::ClientRequest() :
	_method(Method::GET),
	_path(""),
	_version("HTTP/1.1"),
	_buffer(nullptr),
	_offset(0),
	_size(0)
{
	_header.insert({ "Host", "" });
	_header.insert({ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36" });
	_header.insert({ "Accept", "*/*" });
	_header.insert({ "Accept-Encoding", "identity" });
	_header.insert({ "Connection", "keep-alive" });
}

ClientRequest::ClientRequest(ClientRequest const &req)
{
}

ClientRequest::~ClientRequest()
{
	if (_buffer)
		delete[] _buffer;
}

ClientRequest &ClientRequest::operator=(ClientRequest const &req)
{
	return *this;
}

/* public */
std::map<std::string, std::string> &ClientRequest::GetHeader()
{
	return _header;
}

std::string ClientRequest::GetStringHeader()
{
	std::string header;

	header = "";
	header += methodToString(_method) + " " + _path + " " + _version + "\r\n";
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); ++it)
		header += it->first + ": " + it->second + "\r\n";
	header += "\r\n";

	return header;
}

bool ClientRequest::IsWriteDone()
{
	return _offset >= _size;
}

void ClientRequest::SetBuffer(std::string buf)
{
	_buffer = new char[buf.length()];
	for (int i = 0; i < buf.length(); ++i)
		_buffer[i] = buf[i];
	_offset = 0;
	_size = buf.length();
}

char *ClientRequest::GetOffset()
{
	return _buffer + _offset;
}

size_t ClientRequest::GetRemainder()
{
	return _size - _offset;
}

void ClientRequest::Send(unsigned long long int offset)
{
	_offset += offset;
}

/* private */
std::string ClientRequest::methodToString(Method m)
{
	switch (m)
	{
		case Method::GET:
			return "GET";

		case Method::HEAD:
			return "HEAD";
	}

	return "";
}
