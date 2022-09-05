#ifndef _TLSSOCKET_H_
# define _TLSSOCKET_H_

# include <openssl/ssl.h>

class TLSSocket
{
	friend class Connection;

private:
	SSL_CTX *_ctx;
	SSL_METHOD *_method;
	char *_subject;
	char *_issuer;
	SSL *_ssl;
};

#endif
