#include "Connection.h"

/* exception */
const char *Connection::InvalidNameServer::what() const throw() {
	return ("Connection: Name or service not known");
}

/* coplien */
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

/* public */
void Connection::SetURL(std::string url)
{
	parseURL(url);
	parseDNS();
}

void Connection::SetMethod(Method method)
{
	_request._method = method;
}

/* private */
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

void Connection::parseDNS()
{
	struct addrinfo hints = {
		.ai_family = AF_INET
	};
	struct addrinfo *result;
	char buffer[32];

	if (getaddrinfo(_request.GetHeader()["Host"].c_str(), NULL, &hints, &result))
	{
		throw InvalidNameServer();
	}

	_address.sin_addr.s_addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr.s_addr;
	freeaddrinfo(result);
}
