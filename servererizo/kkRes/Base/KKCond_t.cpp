#include "KKCond_t.h"
namespace kkBase
{
CKKCond_t::CKKCond_t(void)
{
#ifdef  USE_WIN_THREAD
	 m_hWait=::CreateEvent(NULL,TRUE,FALSE,NULL);//手动复位，初始状态无效
#else
	//m_hWait=0;
	 pthread_cond_init(&m_hWait,NULL);
#endif 
}

CKKCond_t::~CKKCond_t(void)
{
#ifdef  USE_WIN_THREAD
	::CloseHandle(m_hWait);
#else
	pthread_cond_destroy(&m_hWait);
#endif
}

int CKKCond_t::ResetCond()
{
#ifdef  USE_WIN_THREAD
	return ::ResetEvent(m_hWait);
#else
	return 0;
#endif
}
int CKKCond_t::SetCond()
{
#ifdef  USE_WIN_THREAD
	return ::SetEvent(m_hWait);
#else
	pthread_cond_broadcast(&m_hWait);
	return 0;//m_hWait;
#endif
}
//struct timespec
int CKKCond_t::WaitCond(int ms)
{
	#ifdef  USE_WIN_THREAD
	return 	::WaitForSingleObject(m_hWait, ms);
	#else

	m_Mutex.Lock();
	//pthread_cond_timedwait
	if(INFINITE==ms){
	    pthread_cond_wait(&m_hWait,&m_Mutex);
	}else{
		timespec tv;
		tv.tv_nsec = (ms % 1000) * 1000000;
        tv.tv_sec = time(NULL) + ms / 1000;
	    pthread_cond_timedwait(&m_hWait,&m_Mutex,&tv);
	}
	m_Mutex.UnLock();
	return 0;
	#endif
}
}