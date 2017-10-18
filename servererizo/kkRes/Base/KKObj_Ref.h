#ifndef KKObjRefImpl_H_
#define KKObjRefImpl_H_
#ifndef WIN32
#include <atomic>
#endif
struct IObjRef
{
	virtual long AddRef() = 0;

	virtual long Release() = 0;
	
	virtual void OnFinalRelease() =0;
};

template<class T>
class TObjRefImpl :  public T
{
public:
	TObjRefImpl():m_cRef(1)
	{
        
	}

	virtual ~TObjRefImpl(){
	}

	//!添加引用
	/*!
	*/
	virtual long AddRef()
	{
		#ifdef WIN32
		return InterlockedIncrement(&m_cRef);
		#else
		return	++m_cRef;
		#endif
	}

	//!释放引用
	/*!
	*/
	virtual long Release()
	{
		#ifdef WIN32
		long lRet = InterlockedDecrement(&m_cRef);
		#else
		long	lRet =--m_cRef;
		#endif
		if(lRet==0)
		{
			OnFinalRelease();
		}
		return lRet;
	}

	//!释放对象
	/*!
	*/
    virtual void OnFinalRelease()
    {
        delete this;
    }
protected:
#ifdef WIN32
	volatile LONG m_cRef;
#else
	std::atomic<int> m_cRef;
#endif
};

#endif