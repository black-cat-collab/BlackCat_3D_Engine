#pragma once

namespace bc
{
	extern bool ReadFileBinary(const char* szFile, unsigned char*& pDataBuffer, long& lFileSize);

	extern bool ReadFileBinary(FILE* pFile, unsigned char*& pDataBuffer, const long& lStart, const long& lReadSize);

	extern bool GetFileExtension(const char* szFile, char*& szFilePath, char*& szFileExt);

	extern bool ZipData(unsigned char* pUnZipDataBuffer, int& nUnZipDataLen, void* pZipData, int& nZipDataLen);

	extern bool UnZipData(unsigned char* pZipDataBuffer, int nZipDataLen, int nUnZipDataLen, void* pUnZipData);

	//按zip内部文件目录进行解压到指定目录
	extern bool UnZipFile(const char* szSrcFilePath, const char* szDestFolder);

	extern void MakeDirs(const char* muldir);
}
