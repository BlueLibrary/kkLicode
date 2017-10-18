
#ifndef KKRWLock_H_
#define KKRWLock_H_

#include "KKBase.h"
namespace kkBase
{
	class CKKRWLock
	{
			public:
				   explicit  CKKRWLock();
				   ~CKKRWLock();
				   int Read();
				   int Write();
				   int Wunlock();
				   int Runlock();
			private:
			#ifdef WIN32
				HANDLE m_hNobodyIsReading;
				HANDLE m_hWritingMutex;
				CRITICAL_SECTION m_csReading;
				unsigned int m_nReaders;
			#else
				pthread_rwlock_t m_plock;
			#endif
	};
	class CKKRWLockGurd
	{
		public:
			CKKRWLockGurd(CKKRWLock& lock,bool isread);
			~CKKRWLockGurd();
	private:
			CKKRWLock* p;
			bool m_isread;
	};
}
#endif