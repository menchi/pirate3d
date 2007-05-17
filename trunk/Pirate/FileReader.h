#ifndef _PIRATE_READ_FILE_H_
#define _PIRATE_READ_FILE_H_

#include "RefObject.h"
#include "pirateString.h"
#include <stdio.h>

namespace Pirate
{

/*!
Class for reading a real file from disk.
*/
class FileReader : public virtual RefObject
{
public:
	FileReader(const wchar_t* fileName);
	FileReader(const c8* fileName);

	~FileReader();

	//! returns how much was read
	s32 Read(void* buffer, u32 sizeToRead);

	//! changes position in file, returns true if successful
	//! if relativeMovement==true, the pos is changed relative to current pos,
	//! otherwise from begin of file
	BOOL Seek(s32 finalPos, BOOL relativeMovement = FALSE);

	//! returns size of file
	s32 GetSize();

	//! returns if file is open
	BOOL IsOpen() const
	{
		return m_pFile != 0;
	}

	//! returns where in the file we are.
	s32 GetPos();

	//! returns name of file
	const c8* GetFileName();

private:
	//! opens the file
	void OpenFile();

	stringc m_Filename;
	FILE* m_pFile;
	s32 m_iFileSize;
};

//! Internal function, please do not use.
FileReader* CreateReadFile(const c8* fileName);

} // end namespace 

#endif