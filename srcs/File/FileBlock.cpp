#include "FileBlock.h"

/* coplien */
FileBlock::FileBlock() :
	_start(0),
	_end(0),
	_status(Status::WAITING)
{
}

FileBlock::FileBlock(const FileBlock &fb)
{
}

FileBlock::~FileBlock()
{
}

FileBlock &FileBlock::operator=(const FileBlock &fb)
{
	return *this;
}
