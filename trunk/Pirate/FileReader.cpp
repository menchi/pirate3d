#include "FileReader.h"

namespace Pirate
{

FileReader::FileReader(const c8* fileName)
	: m_pFile(0), m_iFileSize(0)
{
#ifdef _DEBUG
	SetDebugName("FileReader");
#endif

	m_Filename = fileName;
	OpenFile();
}



FileReader::~FileReader()
{
	if (m_pFile)
		fclose(m_pFile);
}



//! returns how much was read
s32 FileReader::Read(void* buffer, u32 sizeToRead)
{
	if (!IsOpen())
		return 0;

	return (s32)fread(buffer, 1, sizeToRead, m_pFile);
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
BOOL FileReader::Seek(s32 finalPos, BOOL relativeMovement)
{
	if (!IsOpen())
		return false;

	return fseek(m_pFile, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}



//! returns size of file
s32 FileReader::GetSize()
{
	return m_iFileSize;
}



//! returns where in the file we are.
s32 FileReader::GetPos()
{
	return ftell(m_pFile);
}



//! opens the file
void FileReader::OpenFile()
{
	if (m_Filename.size() == 0) // bugfix posted by rt
	{
		m_pFile = 0;
		return;
	}

	fopen_s(&m_pFile, m_Filename.c_str(), "rb");

	if (m_pFile)
	{
		// get FileSize

		fseek(m_pFile, 0, SEEK_END);
		m_iFileSize = ftell(m_pFile);
		fseek(m_pFile, 0, SEEK_SET);
	}
}



//! returns name of file
const c8* FileReader::GetFileName()
{
	return m_Filename.c_str();
}



FileReader* CreateReadFile(const c8* fileName)
{
	FileReader* file = new FileReader(fileName);
	if (file->IsOpen())
		return file;

	file->Drop();
	return 0;
}


} // end namespace