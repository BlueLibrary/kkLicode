

#ifndef TranServer_H_
#define TranServer_H_
#include "defkkkcp.h"
#include <map>
#include <set>
#include <string>
#include "kkRes/socket/kksocket.h"
#include "kkRes/Base/KKRWLock.h"

#include "KKTwoKeyMap.h"
typedef struct	TranInfo
{
	/****实际IP地址****/
	KKAddress      Addr;
	/******转发ID*******/
	unsigned int   Id;
	unsigned long long Tick;
	TranInfo();
}	TranInfo;
//udp  转发服务器
class CKKTranServer
{
public:
	    CKKTranServer();
		~CKKTranServer();
		int KkBind(char *Ip,unsigned short port);
		void Start();
protected:
	    static unsigned __stdcall RecUdpThread(void *pUserData);
		void HandleRecUdp();
		void OnTranBufHandle(unsigned char* buf,unsigned short buflen,KKAddress &addr);
		void SendTranId(unsigned int Id,KKAddress &add);
		void SendNotifyCode(int errcode,unsigned int Id,KKAddress &add);

		void FreeTranInfo(std::map<std::string,TranInfo* >::iterator &KcpIt,bool NeedMsg);
private:
	    unsigned int   m_Id;
	    kkBase::CKKRWLock m_rwlock;
	    TKKTwoKeyMap<std::string,unsigned int,TranInfo*> m_TranInfoMap;
		std::set<unsigned int> m_IdSet;
	    volatile int     m_PreOk;
	    kkBase::ckksocket        m_kksocket;
	    KK_THREAD_INFO m_RecThread_info;
};
#endif