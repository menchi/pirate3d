#ifndef _PIRATE_FILE_SYSTEM_H_
#define _PIRATE_FILE_SYSTEM_H_

#include "FileReader.h"

namespace Pirate
{

const s32 FILE_SYSTEM_MAX_PATH = 1024;

/*!
FileSystem which uses normal files and one zipfile
*/
class FileSystem : public virtual RefObject
{
public:

	//! constructor
	FileSystem();

	//! destructor
	~FileSystem();

	//! opens a file for read access
	FileReader* CreateAndOpenFile(const c8* filename);
/*
	//! Creates an IReadFile interface for accessing memory like a file.
	FileReader* CreateMemoryReadFile(void* memory, s32 len, const c8* fileName, BOOL deleteMemoryWhenDropped = FALSE);

	//! Opens a file for write access.
	virtual IWriteFile* createAndWriteFile(const c8* filename, bool append=false);

	//! adds an zip archive to the filesystem
	virtual bool addZipFileArchive(const c8* filename, bool ignoreCase = true, bool ignorePaths = true);
	virtual bool addFolderFileArchive(const c8* filename, bool ignoreCase = true, bool ignorePaths = true);

	//! adds an pak archive to the filesystem
	virtual bool addPakFileArchive(const c8* filename, bool ignoreCase = true, bool ignorePaths = true);
*/
	//! Returns the string of the current working directory
	const c8* GetWorkingDirectory();

	//! Changes the current Working Directory to the string given.
	//! The string is operating system dependent. Under Windows it will look
	//! like this: "drive:\directory\sudirectory\"
	BOOL ChangeWorkingDirectoryTo(const c8* newDirectory);

	//! Converts a relative path to an absolute (unique) path, resolving symbolic links
	stringc GetAbsolutePath(stringc &filename);
/*
	//! Creates a list of files and directories in the current working directory 
	//! and returns it.
	IFileList* createFileList();
*/
	//! determinates if a file exists and would be able to be opened.
	BOOL ExistFile(const c8* filename);
/*
	//! Creates a XML Reader from a file.
	virtual IXMLReader* createXMLReader(const c8* filename);

	//! Creates a XML Reader from a file.
	virtual IXMLReader* createXMLReader(IReadFile* file);

	//! Creates a XML Reader from a file.
	virtual IXMLReaderUTF8* createXMLReaderUTF8(const c8* filename);

	//! Creates a XML Reader from a file.
	virtual IXMLReaderUTF8* createXMLReaderUTF8(IReadFile* file);

	//! Creates a XML Writer from a file.
	virtual IXMLWriter* createXMLWriter(const c8* filename);

	//! Creates a XML Writer from a file.
	virtual IXMLWriter* createXMLWriter(IWriteFile* file);

	//! Creates a new empty collection of attributes, usable for serialization and more.
	virtual IAttributes* createEmptyAttributes(video::IVideoDriver* driver);
*/
private:

//	core::array<CZipReader*> ZipFileSystems;
//	core::array<CPakReader*> PakFileSystems;
//	core::array<CUnZipReader*> UnZipFileSystems;
	c8 m_pWorkingDirectory[FILE_SYSTEM_MAX_PATH];
};


} // end namespace

#endif