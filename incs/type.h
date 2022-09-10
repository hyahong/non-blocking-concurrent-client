#ifndef _TYPE_H_
# define _TYPE_H_

/* macro */
# define RECEIVE_BUFFER_SIZE (1024) /* byte */
# define FILE_BLOCK_SIZE 4//(20 * 1024 * 1024) /* byte */
# define WORKER_NUMBER 10

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

enum class Mode 
{
	BLOCKING,
	NONBLOCKING
};

enum class Status
{
	WAITING,
	RUNNING,
	FAILED
};

#endif
