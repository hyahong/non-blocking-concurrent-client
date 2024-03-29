#include "FileBlock.h"

/* coplien */
FileBlock::FileBlock() :
	_isCached(false),
	_start(0),
	_end(0),
	_stackedSize(0),
	_status(Status::WAITING)
{
}

FileBlock::FileBlock(const FileBlock &fb)
{
	(void) fb;
}

FileBlock::~FileBlock()
{
}

FileBlock &FileBlock::operator=(const FileBlock &fb)
{
	(void) fb;
	return *this;
}

/* public */
unsigned long long int FileBlock::GetStart()
{
	return _start;
}

unsigned long long int FileBlock::GetEnd()
{
	return _end;
}

void FileBlock::SetStackedSize(unsigned long long int size)
{
	_stackedSize = size;
}

unsigned long long int FileBlock::GetStackedSize()
{
	return _stackedSize;
}
