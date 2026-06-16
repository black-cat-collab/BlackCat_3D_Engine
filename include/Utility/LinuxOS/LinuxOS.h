#pragma once


#ifdef __GNUC__	

extern unsigned long long GetTickCount64();
extern unsigned long GetTickCount();

extern int GetModuleFileName(void* handle,char *pathbuf, int buflen);

extern short GetAsyncKeyState(int vKey);
extern bool  GetKeyboardState(unsigned char *keyBuffer);




	
	
#endif
	






