#ifndef KKBase_H_
#define KKBase_H_
///一个跨平台的ku
#ifdef _WINDOWS
#ifndef USE_WIN_THREAD
#define USE_WIN_THREAD
#endif
#endif
      
#ifdef USE_WIN_THREAD
   #include <Windows.h>
   #include <process.h>
   #include <assert.h>
#endif

typedef unsigned short                  kk_ushort;
typedef unsigned long                   kk_ulong;

typedef unsigned long long              kkUInt64;
typedef long long                       kkInt64;

#define KK_MAKELONG(low,high)           ((LONG)(((kk_ushort)(low & 0xffff)) | ((kk_ulong)((kk_ushort)((high & 0xffff))) << 16)))

#define KK_LO_USHORT(l)                 ((kk_ushort)(((kk_ulong)(l)) & 0xffff))
#define KK_HI_USHORT(l)                 ((kk_ushort)((((kk_ulong)(l)) >> 16) & 0xffff))


#ifndef _WINDOWS
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
		typedef int                 BOOL;
		typedef void *HANDLE;
		typedef HANDLE HWND;
		typedef unsigned long       DWORD;

		typedef unsigned int UINT;
		typedef int HDC;
		typedef long LONG;
		typedef void            *LPVOID;
		typedef long long      INT64;
		typedef struct tagRECT
		{
			LONG    left;
			LONG    top;
			LONG    right;
			LONG    bottom;
		} RECT;
        typedef unsigned char       BYTE;
		typedef unsigned long ULONG_PTR;
		typedef ULONG_PTR DWORD_PTR;
		typedef unsigned short      WORD;

        #define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
		#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
		#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
		#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
		#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
		#define INFINITE            0xFFFFFFFF  // Infinite timeout
        #define TRUE                1
#endif

//////线程信息
typedef struct KK_THREAD_INFO
{
#ifdef _WINDOWS
	void*         ThreadHandle;
#else
	pthread_t     Tid_task;
#endif
	unsigned      ThreadAddr;
	volatile bool ThreadExit;
}KK_THREAD_INFO;
///休眠函数
void                      KKSleep(int mi);
kkUInt64                  KKTickCount();

void*                     KKmalloc(size_t _Size);
void                      KKfree(void * _Memory);
#endif