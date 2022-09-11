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

	unsigned long long int GetStart();
	unsigned long long int GetEnd();

	void SetStackedSize(unsigned long long int size);
	unsigned long long int GetStackedSize();

private:
	bool _isCached;

	unsigned long long int _start;
	unsigned long long int _end;

	unsigned long long int _stackedSize;

	Status _status;
};

#endif
