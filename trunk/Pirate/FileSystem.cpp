#include "FileSystem.h"
#include "OS.h"
#include <stdio.h>

#ifdef _PIRATE_WINDOWS_
#include <direct.h> // for _chdir
#endif

namespace Pirate
{

//! constructor
FileSystem::FileSystem()
{
#ifdef _DEBUG
	SetDebugName("FileSystem");
#endif
}



//! destructor
FileSystem::~FileSystem()
{
}


//! opens a file for read access
FileReader* FileSystem::CreateAndOpenFile(const c8* filename)
{
	FileReader* file = 0;

	file = CreateReadFile(filename);
	return file;
}

//! Returns the string of the current working directory
const c8* FileSystem::GetWorkingDirectory()
{
#ifdef _PIRATE_WINDOWS_
	_getcwd(m_pWorkingDirectory, FILE_SYSTEM_MAX_PATH);
#endif

	return m_pWorkingDirectory;
}


//! Changes the current Working Directory to the string given.
//! The string is operating system dependent. Under Windows it will look
//! like this: "drive:\directory\sudirectory\"
//! \return
//! Returns true if successful, otherwise false.
BOOL FileSystem::ChangeWorkingDirectoryTo(const c8* newDirectory)
{
	BOOL success=FALSE;
#ifdef _PIRATE_WINDOWS_
	success=(_chdir(newDirectory) == 0);
#endif

	return success;
}

stringc FileSystem::GetAbsolutePath(stringc &filename)
{
	c8 *p=0;
	stringc ret;

#ifdef _PIRATE_WINDOWS_

	c8 fpath[_MAX_PATH];
	p = _fullpath( fpath, filename.c_str(), _MAX_PATH);
	ret = p;

#endif

	return ret;
}

//! determines if a file exists and would be able to be opened.
BOOL FileSystem::ExistFile(const c8* filename)
{
	FILE* f; 
	fopen_s(&f, filename, "rb");
	if (f) 
	{
		fclose(f);
		return TRUE;
	}

	return FALSE;
}

}