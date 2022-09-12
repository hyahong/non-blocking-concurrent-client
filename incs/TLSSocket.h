#ifndef _TLSSOCKET_H_
# define _TLSSOCKET_H_

# include <openssl/ssl.h>

class TLSSocket
{
	friend class Connection;

private:
	char *_subject;
	char *_issuer;
	SSL *_ssl;
};

#endif
