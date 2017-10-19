#ifndef WIN32_KK
    #ifdef Android_Plat
         #include <android/log.h>
         #include <pthread.h>
         #include <unistd.h>
         // #define  LOGE(...) ; 
         #define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
    #elif  _WINDOWS_PTHREAD
        #include <Windows.h>
		#include <assert.h>
        #include "../WinPthread/include/pthread.h"
        #pragma comment (lib,"..\\WinPthread\\lib\\pthreadVC2.lib")
        #define LOGE(...) ;
   #endif
    typedef pthread_rwlock_t KKCRITICAL_SECTION;
   
    #define NULL 0
	#define ANDROID_KKPAYER 1
	#define INFINITE            0xFFFFFFFF  // Infinite timeout
	#define FALSE               0
	#define TRUE                1
	#ifndef WIN32
		 #define __stdcall __attribute__((__stdcall__))
	#endif
#endif