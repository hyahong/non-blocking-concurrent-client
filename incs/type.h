#ifndef _TYPE_H_
# define _TYPE_H_

/* error type */
# define NO_ERROR 0
# define INVALID_NAMESERVER -1

/* enum type */
enum class Schema
{
	HTTP,
	HTTPS
};

enum class Method
{
	GET,
	HEAD
};

#endif
