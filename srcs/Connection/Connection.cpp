#include "Connection.h"

Connection::Connection()
{
}

Connection::Connection(Connection const &conn)
{
}

Connection::~Connection()
{
}

Connection &Connection::operator=(Connection const &conn)
{
	return *this;
}

int Connection::SetURL(std::string url)
{
	int error;

	parseURL(url);
	error = parseDNS();

	return error;
}

void Connection::SetMethod(Method method)
{
	_request._method = method;
}

void Connection::parseURL(std::string url)
{
	size_t cursor;

	cursor = 0;
	_schema = Schema::HTTPS;
	if (!url.compare(cursor, strlen("http://"), "http://") || !url.compare(cursor, strlen("https://"), "https://"))
	{
		_schema = !url.compare(cursor, strlen("http://"), "http://") ? Schema::HTTP : Schema::HTTPS;
		cursor += _schema == Schema::HTTP ? strlen("http://") : strlen("https://");
	}
	_request.GetHeader()["Host"] = url.substr(cursor, url.find("/", cursor) - cursor);
	cursor += _request.GetHeader()["Host"].length();
	_request._path = url.substr(cursor);
	if (_request._path.empty())
		_request._path = "/";
}

int Connection::parseDNS()
{
	struct addrinfo hints = {
		.ai_family = AF_INET
	};
	struct addrinfo *result;
	char buffer[32];

	if (getaddrinfo(_request.GetHeader()["Host"].c_str(), NULL, &hints, &result))
	{
		return INVALID_NAMESERVER;
	}

	_address.sin_addr.s_addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr.s_addr;
	freeaddrinfo(result);

	return NO_ERROR;
}
