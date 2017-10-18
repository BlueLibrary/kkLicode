#ifndef KKMutex_H_
#define KKMutex_H_
#include "KKBase.h"
namespace kkBase
{
	class CKKMutex
	{
		public:
				CKKMutex();
				~CKKMutex();
				void Lock();
				void UnLock();

				#ifndef  USE_WIN_THREAD
	               pthread_mutex_t* operator&();
	            #endif
		private:
	#ifdef  USE_WIN_THREAD
				CRITICAL_SECTION criticalSection;
	#else
				pthread_mutex_t m_KKMutex;
	#endif
	};

	class CKKGurd
	{
	  public:
				CKKGurd(CKKMutex *Mutex)
				{
					m_pMutex=Mutex;
					if(m_pMutex!=NULL)
					  m_pMutex->Lock();
				}
				~CKKGurd()
				{
					if(m_pMutex!=NULL)
						m_pMutex->UnLock();
				}
	private:
			   void* operator new(size_t size );
			   void  operator delete(void *ptr);
				CKKGurd(const CKKGurd& cs);
				CKKGurd operator = (const CKKGurd& cs);
	private:
				CKKMutex *m_pMutex;
	};
}
#endif