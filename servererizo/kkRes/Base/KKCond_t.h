
#ifndef KKCond_t_H_
#define KKCond_t_H_
#include "KKMutex.h"
namespace kkBase
{
//条件变量实现
class CKKCond_t
{
    public:
	       CKKCond_t(void);
	       ~CKKCond_t(void);
		   //重置
           int ResetCond();
		   int SetCond();
		   int WaitCond(int ms);
private:
#ifdef USE_WIN_THREAD
		HANDLE m_hWait;
#else
		pthread_cond_t  m_hWait;
        CKKMutex        m_Mutex;
#endif
};
}
#endif
