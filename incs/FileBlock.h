#ifndef _FILEBLOCK_H_
# define _FILEBLOCK_H_

# include <string>
# include "type.h"

class FileBlock
{
	friend class Cluster;

public:
	/* coplien */
	FileBlock();
	FileBlock(const FileBlock &fb);
	virtual ~FileBlock();

	FileBlock &operator=(const FileBlock &fb);

private:
	unsigned long long int _start;
	unsigned long long int _end;

	Status _status;
};

#endif
