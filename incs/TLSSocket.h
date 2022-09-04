#ifndef _TLSSOCKET_H_
# define _TLSSOCKET_H_

# include <openssl/ssl.h>

class TLSSocket
{
public:
	SSL_CTX * ctx;
	SSL_METHOD * method;
	char * subject;
	char * issuer;
	SSL * ssl;
};

#endif
