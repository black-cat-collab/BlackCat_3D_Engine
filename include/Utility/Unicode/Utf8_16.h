#pragma once
#include "BasicDef.h"
#include <assert.h>

namespace bc
{
	enum class UniMode 
	{ 
		uni8Bit = 0, uniUTF8 = 1, uni16BE = 2, uni16LE = 3, uniCookie = 4, uni7Bit = 5, uni16BE_NoBOM = 6, uni16LE_NoBOM = 7, uniEnd 
	};
	enum class u78 
	{ 
		utf8NoBOM = 0, ascii7bits = 1, ascii8bits = 2 
	};

	class Utf8_16 {
	public:
		static const byte k_Boms[(ulong)UniMode::uniEnd][3];
	};
	
	// Reads UTF-8 and outputs UTF-16
	class Utf8_Iter : public Utf8_16 {
	public:
		Utf8_Iter();
		void reset();
		void set(const byte* pBuf, size_t nLen, UniMode eEncoding);
		ushort get() const {
#ifdef _DEBUG
			assert(m_eState == eState::eStart);
#endif
			return m_nCur;
		}
		bool canGet() const { return m_eState == eState::eStart; }
		void operator++();
		operator bool() { return m_pRead <= m_pEnd; }

	protected:
		void swap();
		void toStart(); // Put to start state, swap bytes if necessary
		
	protected:
		enum class eState {
			eStart,
			e2Bytes_Byte2,
			e3Bytes_Byte2,
			e3Bytes_Byte3
		};

		UniMode m_eEncoding;
		eState m_eState;
		ushort m_nCur;
		const byte* m_pBuf;
		const byte* m_pRead;
		const byte* m_pEnd;
	};

	// Reads UTF-16 and outputs UTF-8
	class Utf16_Iter : public Utf8_16
	{
	public:
		Utf16_Iter();
		void reset();
		void set(const byte* pBuf, size_t nLen, UniMode eEncoding);
		byte get() const { return m_nCur; };
		void operator++();
		operator bool() { return m_pRead <= m_pEnd; };

	protected:
		void toStart(); // Put to start state, swap bytes if necessary
	protected:
		enum class eState {
			eStart,
			e2Bytes2,
			e3Bytes2,
			e3Bytes3
		};

		UniMode m_eEncoding;
		Utf16_Iter::eState m_eState;
		byte m_nCur;
		ushort m_nCur16;
		const byte* m_pBuf;
		const byte* m_pRead;
		const byte* m_pEnd;
	};

	// Reads UTF16 and outputs UTF8
	class Utf8_16_Read : public Utf8_16 {
	public:
		Utf8_16_Read();
		~Utf8_16_Read();

		size_t convert(char* buf, size_t len);
		char* getNewBuf() { return reinterpret_cast<char*>(m_pNewBuf); }
		UniMode getEncoding() const { return m_eEncoding; }
		static UniMode determineEncoding(const byte* buf, size_t bufLen);
	protected:
		void determineEncoding();

		u78 utf8_7bits_8bits();
	private:
		UniMode			m_eEncoding;
		byte*			m_pBuf;
		byte*			m_pNewBuf;
		size_t          m_nBufSize;
		size_t			m_nSkip;
		bool            m_bFirstRead;
		size_t          m_nLen;
		Utf16_Iter      m_Iter16;
	};

	// Read in a UTF-8 buffer and write out to UTF-16 or UTF-8
	class Utf8_16_Write : public Utf8_16 {
	public:
		Utf8_16_Write();
		~Utf8_16_Write();

		void setEncoding(UniMode eType);

		FILE* fopen(const wchar_t* _name, const wchar_t* _type);
		//size_t fwrite(const void* p, size_t _size);
		void   fclose();

		size_t convert(char* p, size_t _size);
		char* getNewBuf() { return reinterpret_cast<char*>(m_pNewBuf); }

	protected:
		UniMode m_eEncoding;
		FILE* m_pFile;
		byte* m_pNewBuf;
		size_t m_nBufSize;
		bool m_bFirstWrite;
	};
}

