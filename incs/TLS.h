#ifndef _TLS_H_
# define _TLS_H_

# include <openssl/ssl.h>

/* singleton */
class TLS
{
	friend class Connection;

public:
	/* coplien */
	TLS()
	{
		SSL_library_init();
		SSL_load_error_strings();

		_method = const_cast<SSL_METHOD *>(TLS_client_method());
		if (!_method)
			throw MethodAllocationFailure();
		_ctx = SSL_CTX_new(_method);
		if (!_ctx)
			throw CTXAllocationFailure();
	}

	TLS(const TLS &t) :
		_method(t._method),
		_ctx(t._ctx)
	{
	}

	virtual	~TLS()
	{
		SSL_CTX_free(_ctx);
	}

	TLS &operator=(const TLS &t)
	{
		_method = t._method;
		_ctx = t._ctx;

		return *this;
	}

	/* exception */
	class MethodAllocationFailure : public std::exception {
		virtual const char *what() const throw()
		{
			return ("Connection: Failed to allocate TLS Method");
		}
	};

	class CTXAllocationFailure : public std::exception {
		virtual const char *what() const throw()
		{
			return ("Connection: Failed to allocate CTX");
		}
	};

	/* static */
	static TLS &GetInstance()
	{
		static TLS _tls;

		return _tls;
	}

private:
	SSL_METHOD *_method;
	SSL_CTX *_ctx;
};

#endif
