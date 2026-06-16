#pragma once
//#ifndef     _XNEW_H_
//#define  	_XNEW_H_

#include <iostream>
#include<list>
//#define _CHECK_MEMORY
#ifdef   _CHECK_MEMORY


void OutMemList(const char* strFile);
void check_memleak();


typedef struct xx_meminfo
{
	int line;
	char filename[256];
	int size;
	void* p;
	struct xx_meminfo* next;

}XX_MEMINFO;


using namespace std;
typedef list<XX_MEMINFO> list_meminfo;
//extern list_meminfo listmeninfo;
//extern  XX_MEMINFO *gList;

bool AddToList(XX_MEMINFO* TheNode);
bool DeleteFromNode(void* p);



///void * operator new(size_t size, void *location);

void* operator new(size_t size, void* p, int type);

void* operator new (size_t& size, const char* filename, int line);
void* operator new (size_t size, const char* filename, int line);
void* operator new[](size_t size, const char* filename, int line);
void* operator new(size_t size, void* p, int type);
void operator delete(void* _Ptr);
void operator delete[](void* _Ptr);




//void* operator new(size_t size, void*p, int type);


//#define new new(__FILE__,__LINE__)
//
//#ifndef NEW_OVERLOAD_IMPLEMENTATION_
//#define new new( __FILE__, __LINE__ )
//// 预定义宏: 
//// __FILE__(两个下划线): 代表当前源代码文件名
//// __LINE__(两个下划线): 代表当前源代码文件中的行号
//#endif


#endif


