#ifndef _FILEBLOCK_H_
# define _FILEBLOCK_H_

# include <string>

class FileBlock
{
public:
	/* coplien */
	FileBlock();
	FileBlock(const FileBlock &fb);
	virtual ~FileBlock();

	FileBlock &operator=(const FileBlock &fb);

private:
};

#endif
