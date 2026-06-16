#pragma once
#include <string>
#include "BasicDef.h"

namespace bc
{
	class KConvert
	{
	public:
		static int To_Int32_Hex(const char* data);
		static int To_Int32(const char* x);
		static int To_Int32(const wchar_t* x);
		static double To_Double(const char* x);
		static float To_Float(const char* x);
		static float To_Float(const wchar_t* x);
		static std::wstring To_wstring(const char* x);
		static std::wstring To_wstring(const ushort* x);
		static std::string To_astring(const wchar_t* x);
		static std::wstring& trim_space(std::wstring& str);
		static std::string To_astring(int v);
		static std::string To_astring(double v);
		static std::wstring To_wstring(int v);
		static std::wstring To_wstring(double v);
		static std::wstring To_wstring_from_utf8(const char* utf8);
		static std::string To_utf8_astring(const wchar_t* unicode);
		static std::string ascii2utf8(const char* x);
		static char* To_LCase(char* x);
		static char* To_UCase(char* x);
		static wchar_t* To_LCase(wchar_t* x);
		static wchar_t* To_UCase(wchar_t* x);
		static std::string To_astring(char* X);


	};

	extern std::wstring _K(const char* str);

	extern size_t	UTF8Length(const char* uptr, size_t tlen);
	extern size_t	UTF8Length(const wchar_t* uptr, size_t tlen);
	extern void		UTF8FromUCS2(const wchar_t* uptr, size_t tlen, char* putf, size_t len);
	extern size_t	UCS2Length(const char* s, size_t len);
	extern size_t	UCS2FromUTF8(const char* s, size_t len, wchar_t* tbuf, size_t tlen);
	extern size_t	AsciiToUtf8(const char* pszASCII, size_t lenASCII, char* pszUTF8);
	extern size_t	AsciiNum(const char* s);
	extern size_t	Utf8ToAscii(const char* pszUTF8, size_t lenUTF8, char* pszASCII);
	extern std::string Utf8ToAscii(const char* pszUTF8);
	extern size_t	Utf8Num(const char* s);
}

