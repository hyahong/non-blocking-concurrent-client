#include "ClientRequest.h"

/* coplien */
ClientRequest::ClientRequest() :
	_method(Method::GET),
	_path(""),
	_version("HTTP/1.1")
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
