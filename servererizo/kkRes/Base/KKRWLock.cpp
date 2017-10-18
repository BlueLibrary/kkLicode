#include "KKRWLock.h"
namespace kkBase
{
		CKKRWLock::CKKRWLock()
		{
		#ifdef WIN32
	
			m_hNobodyIsReading = CreateEvent(NULL, TRUE, TRUE, NULL);
			m_hWritingMutex = CreateMutex(NULL, FALSE, NULL);
			assert(m_hNobodyIsReading && m_hWritingMutex);
			InitializeCriticalSection(&m_csReading);
			m_nReaders = 0;
		#else
			 pthread_rwlock_init(&m_plock, NULL);
		#endif
		}
		CKKRWLock::~CKKRWLock()
		{
		#ifdef WIN32
			CloseHandle(m_hNobodyIsReading);
			CloseHandle(m_hWritingMutex);
			DeleteCriticalSection(&m_csReading);
			assert(m_nReaders == 0);
		#else
			pthread_rwlock_destroy(&m_plock);
		#endif
		}
		int CKKRWLock::Read()
		{
		#ifdef WIN32
	
			// We make sure that nobody is writing
			if (WaitForSingleObject(m_hWritingMutex, INFINITE) != WAIT_OBJECT_0)
				return FALSE;

			// We need a critical section to update the number of readers
			EnterCriticalSection(&m_csReading);
			{
				m_nReaders++;
				if (m_nReaders == 1)
				{
					// We must indicate that there are some readers
					ResetEvent(m_hNobodyIsReading);
				}
			}
			LeaveCriticalSection(&m_csReading);


			ReleaseMutex(m_hWritingMutex);
			return 1;
		#else
			return pthread_rwlock_rdlock(&m_plock);
		#endif
		}
		int CKKRWLock::Write()
		{
		#ifdef WIN32
			// Only one writer at a time
			if (WaitForSingleObject(m_hWritingMutex,INFINITE) != WAIT_OBJECT_0)
				return FALSE;

			// Wait for all readers to leave
			if (WaitForSingleObject(m_hNobodyIsReading,INFINITE) != WAIT_OBJECT_0)
			{
				// We have waited too long, so we have to set the "Writing" mutex
				ReleaseMutex(m_hWritingMutex);
				return FALSE;
			}

			return TRUE;
		#else
			 return pthread_rwlock_wrlock(&m_plock);
		#endif
		}

		int CKKRWLock::Wunlock()
		{
		#ifdef WIN32
			 return ReleaseMutex(m_hWritingMutex);
	
		#else
			return pthread_rwlock_unlock(&m_plock);
		#endif
		}

		int CKKRWLock::Runlock()
		{
		#ifdef WIN32
			// We need a critical section to update the number of readers
			EnterCriticalSection(&m_csReading);
			{
				m_nReaders--;
				if (m_nReaders == 0)
				{
					// We indicate that there are no more readers
					SetEvent(m_hNobodyIsReading);
				}
			}
			LeaveCriticalSection(&m_csReading);
			return 0;
		#else
			return pthread_rwlock_unlock(&m_plock);
		#endif
		}


		CKKRWLockGurd::CKKRWLockGurd(CKKRWLock& lock,bool isread):p(&lock),m_isread(isread)
		{

			if(isread)
			{
				lock.Read();
			}else{
				lock.Write();
			}

		}
		CKKRWLockGurd::~CKKRWLockGurd()
		{
			if(m_isread)
			{
				p->Runlock();
			}else{
				p->Wunlock();
			}
		}
}
