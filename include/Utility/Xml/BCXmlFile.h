#pragma once
#include "BasicDef.h"

namespace bc
{
	class BCXmlFile
	{
	public:
		BCXmlFile();
		virtual ~BCXmlFile();
		char*	GetData() { return m_pDateBuff; }
		uint	GetLength() { return m_nLength; }

		size_t GetUtf8Buff(char* pBuff, int nMaxLengh);
		size_t	GetBufferLength() { return m_nMaxLength; }
		bool	AllocBuffer(const size_t& bufferSizeRequested);
		bool	AppendText(const char* pData, const size_t& nLength);
		bool	Input(const byte* pData, const size_t& fileSize, int& encoding);
		bool	IsExist(const char* fileName);
		bool	Load(const char* fileName, bool bEncoding = false);

		size_t	ToASCII(char* pAsciiBuff);
		size_t	ToUCS2(wchar_t* pUCS2, size_t nLen);
		size_t	ToUCS2(ushort* pUCS2, size_t nLen);
		size_t	GetUCS2Leng();
		int		GetHtmlXmlEncoding(const char* fileName) const;
		bool	LoadEx(const char* fileName);
	private:
		char*				m_pDateBuff;
		size_t				m_nMaxLength;
		uint				m_nLength;
		int					m_nSkip;
	};
}


