#ifndef _TYPE_H_
# define _TYPE_H_

/* macro */
# define RECEIVE_BUFFER_SIZE (4096) /* byte */
# define FILE_BLOCK_SIZE_DEFAULT (20 * 1024 * 1024) /* byte */
# define WORKER_NUMBER_DEFAULT (10)

# define PROGRESS_BAR_WIDTH (20)
# define PROGRESS_COLUMN (8)
# define PROGRESS_MARGIN (5)

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
	DONE,
	FAILED
};

#endif
