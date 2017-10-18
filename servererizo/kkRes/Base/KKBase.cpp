#include "KKBase.h"
#ifdef WIN32
#include <time.h>
#else
#include<sys/time.h>
#endif
void KKSleep(int mi)
{
	#ifdef WIN32
		Sleep(mi);
	#else
		usleep(mi*1000);
	#endif
}
static inline void itimeofday(long *sec, long *usec)
{
	#ifdef WIN32
		static long mode = 0, addsec = 0;
		BOOL retval;
		static long long freq = 1;
		long long qpc;
		if (mode == 0) {
			retval = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			freq = (freq == 0)? 1 : freq;
			retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
			addsec = (long)time(NULL);
			addsec = addsec - (long)((qpc / freq) & 0x7fffffff);
			mode = 1;
		}
		retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
		retval = retval * 2;
		if (sec) 
			*sec = (long)(qpc / freq) + addsec;
		if (usec) 
			*usec = (long)((qpc % freq) * 1000000 / freq);
	#else
		struct timeval time;
		gettimeofday(&time, NULL);
		if (sec) 
			*sec = time.tv_sec;
		if (usec) 
			*usec = time.tv_usec;
	#endif
}
static inline long long iclock64(void)
{
	long s, u;
	long long value;
	itimeofday(&s, &u);
	value = ((long long)s) * 1000 + (u / 1000);
	return value;
}

static inline unsigned int iclock()
{
	return (unsigned int)(iclock64() & 0xfffffffful);
}

unsigned long long  KKTickCount()
{
#ifdef WIN32
	return ::GetTickCount64();
#else
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

void *KKmalloc(size_t _Size)
{
	return malloc(_Size);
}
void  KKfree(void * _Memory)
{
	free(_Memory);
}