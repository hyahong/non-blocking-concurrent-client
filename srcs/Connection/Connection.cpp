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

bool Connection::SetURL(std::string url)
{
	// url parse 
	// dns parse
	std::cout << url << std::endl;
	parseURL(url);
	
	return true;
}

void Connection::SetMethod(Method method)
{
	_request._method = method;
}

bool Connection::parseURL(std::string url)
{
	size_t cursor;

	cursor = 0;
	_schema = Schema::HTTPS;
	if (!url.compare(cursor, strlen("http://"), "http://") || !url.compare(cursor, strlen("https://"), "https://"))
	{
		_schema = !url.compare(cursor, strlen("http://"), "http://") ? Schema::HTTP : Schema::HTTPS;
		cursor += _schema == Schema::HTTP ? strlen("http://") : strlen("https://");
	}
	//_request = url.substr(cursor, url.find("/", cursor) - cursor);
	std::cout << url.substr(cursor, url.find("/", cursor) - cursor) << std::endl;
	return true;
}
