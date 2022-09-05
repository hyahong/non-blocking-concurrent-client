#include "Connection.h"

/* exception */
const char *Connection::InvalidNameServer::what() const throw() {
	return ("Connection: Name or service not known");
}

const char *Connection::ConnectionFailure::what() const throw() {
	return ("Connection: Failed to connect to server");
}

const char *Connection::CTXAllocationFailure::what() const throw() {
	return ("Connection: Failed to allocate CTX");
}

const char *Connection::SSLAllocationFailure::what() const throw() {
	return ("Connection: Failed to allocate SSL");
}

const char *Connection::FailureToUseSNI::what() const throw() {
	return ("Connection: Failed to use SNI");
}

const char *Connection::FailureToSetFD::what() const throw() {
	return ("Connection: SSL socket setup failed");
}

const char *Connection::ConnectSSLFailure::what() const throw() {
	return ("Connection: Failed to connect by SSL");
}

/* coplien */
Connection::Connection()
{
	bzero(&_address, sizeof(_address));
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

void Connection::Connect()
{
	/* socket */
	_socket = ::socket(PF_INET, SOCK_STREAM, 0);
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_schema == Schema::HTTP ? 80 : 443);
	
	/* connect */
	if (::connect(_socket, (struct sockaddr *) &_address, sizeof(_address)) != 0)
		throw ConnectionFailure();

	/* set TLS if needed */
	if (_schema == Schema::HTTPS)
		useTLS();
}

ClientRequest &Connection::GetRequest()
{
	return _request;
}

ClientResponse &Connection::GetResponse()
{
	return _response;
}

/* I/O */
ssize_t Connection::write(const void *buf, size_t count)
{
	if (_schema == Schema::HTTP)
		return ::write(_socket, buf, count);
	if (_schema == Schema::HTTPS)
		return SSL_write(_tls._ssl, buf, count);
	return -1;
}

ssize_t Connection::read(void *buf, size_t count)
{
	if (_schema == Schema::HTTP)
		return ::read(_socket, buf, count);
	if (_schema == Schema::HTTPS)
		return SSL_read(_tls._ssl, buf, count);
	return -1;
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
	/* get schema */
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

	/* dns to ipv4 */
	if (getaddrinfo(_request.GetHeader()["Host"].c_str(), NULL, &hints, &result))
		throw InvalidNameServer();

	_address.sin_addr.s_addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr.s_addr;
	freeaddrinfo(result);
}

void Connection::useTLS()
{
	X509 *cert;

	SSL_library_init();
	SSL_load_error_strings();
	/* ssl base */
	_tls._method = const_cast<SSL_METHOD *>(TLS_client_method());
	_tls._ctx = SSL_CTX_new(_tls._method);
	if (!_tls._ctx)
		throw CTXAllocationFailure();
	_tls._ssl = SSL_new(_tls._ctx);
	if (!_tls._ssl)
		throw SSLAllocationFailure();
	if (!SSL_set_tlsext_host_name(_tls._ssl, _request.GetHeader()["Host"].c_str()))
		throw FailureToUseSNI();
	if (!SSL_set_fd(_tls._ssl, _socket))
		throw FailureToSetFD();
	if (SSL_connect(_tls._ssl) <= 0)
		throw ConnectSSLFailure();
	/* certification */
	cert = SSL_get_peer_certificate(_tls._ssl);
	if (cert)
	{
		_tls._subject = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		_tls._issuer = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		X509_free(cert);
	}
}
