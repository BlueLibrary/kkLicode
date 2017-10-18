#include "KKMutex.h"
namespace kkBase
{
	CKKMutex::CKKMutex(void)
	{
	#ifdef  USE_WIN_THREAD
		InitializeCriticalSection(&criticalSection);
	#else
		pthread_mutex_init(&m_KKMutex, NULL);
	#endif
	}

	void CKKMutex::Lock()
	{
	#ifdef  USE_WIN_THREAD
		//::WaitForSingleObject(m_KKMutex, INFINITE);
		EnterCriticalSection(&criticalSection);
	#else
		pthread_mutex_lock(&m_KKMutex);
	#endif
	}
	void CKKMutex::UnLock()
	{
		#ifdef  USE_WIN_THREAD
			LeaveCriticalSection(&criticalSection);
		#else
			 pthread_mutex_unlock(&m_KKMutex);
		#endif
	}
	#ifndef  USE_WIN_THREAD
	pthread_mutex_t* CKKMutex::operator&()
	{
		return &m_KKMutex;
	}
	#endif

	CKKMutex::~CKKMutex(void)
	{
		#ifdef  USE_WIN_THREAD
			DeleteCriticalSection(&criticalSection);
		#else
			pthread_mutex_destroy(&m_KKMutex);
		#endif	
	}


	void* CKKGurd::operator new(size_t size )
	{
		return NULL;
	}
	void  CKKGurd::operator delete(void *ptr)
	{
     
	}
}