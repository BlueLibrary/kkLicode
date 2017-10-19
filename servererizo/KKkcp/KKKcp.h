#include "defkkkcp.h"
#include <map>
#include <queue>
#include <string>
#include "kkRes/socket/kksocket.h"
#include "kkRes/Base/KKMutex.h"
#include "kkRes/Base/KKRWLock.h"


#include "KKTwoKeyMap.h"
#ifndef Ckkkcp_h_
#define Ckkkcp_h_
#ifndef _MSC_VER
		 #define __stdcall __attribute__((__stdcall__))
#endif
class CKKkcp
{
public: 
	    CKKkcp();
		~CKKkcp();
		static int GetIpV4ByHostName(const char *host_name,std::string &OutIp);
public:
	    void Start(bool AutomaticHeart=true);
		/*****转发者******/
		int SetTracker(std::string Ip,int port);
		/****绑定Ip***/
		int KkBind(const char *Ip,unsigned short port);
		
        //// 流式接口需要建立连接了才能发送
        int                    StablySend(IUINT32 Id,const char* buf,int buflen);

		///ConId，客户端需要连接了才能发送。
		int                    UdpSendByConId(IUINT32 ConId,const char* buf,int buflen);
		int                    UdpSendByAddr(KKAddress& Addr,const char* buf,int buflen);
		unsigned short         GetMtuSize(IUINT32 ConId);
		/********链接远程**********/
		int Connect(std::string Ip,int port);
		//通过转发服务器连接目的端
		int Connect(std::string TranIp,kk_ushort port,kk_ulong nDestTranId);
		/*******链接转发服务器*********/
		int ConnectTranServer(std::string Ip,kk_ushort port);
		
		/********得到一个转发服务器TranId*********/
		IUINT32 GetOwerTranId(std::string Ip,int port);
		//断开连接
		int DisConnect(IUINT32 Id);
		//断开连接
		int DisConnect(std::string Ip,int port,kk_ulong nDestTranId);
		/********获取接收的数据包*********/
		KK_Packet* Receive();
		void DelPacket(KK_Packet* packet);
private:
/********使用kcp发送数据，发送失败返回-1，buflen 最大值2000*********/
		int KKSend(IUINT32 Id,const char* buf,unsigned short buflen);
		IUINT32        m_nLocalTranId;
		///控制命令线程，是否考虑和以下两个线程合并
		static unsigned __stdcall CmdThread(void *pUserData);
		void HandleCmd();	

		///Udp接收数据处理线程
		static unsigned __stdcall RecUdpThread(void *pUserData);
		void HandleRecUdp();

		///链接管理线程
		static unsigned __stdcall ConnMgrThread(void *pUserData);
		void ConnMgrThread2();

		void OnConHandle(unsigned char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr); 
		void OnKcp(unsigned char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr); 
		ikcpcb* CreateKcp(std::string Ipport,IUINT32 cov,unsigned short mtu);//ikcpcb *kcp = ikcp_create(conv, user);

		void OnBufHandle(char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr);
		void OnTranBufHandle(unsigned char* buf,unsigned short buflen,KKAddress &addr);
		/******放一个接收的数据包*******/
		void PutKKPacket(KK_Packet* packet);
		void GetRecKcpData(ikcpcb *kcp,KKAddress& Addr);
		void GenSysKKPacket(KKConState sys,KKAddress& Addr,unsigned long conId);
		/************释放*************/
		void FreeKcpInfo(std::map<std::string,KcpInfo* >::iterator &KcpIt,bool NeedMsg=true);

		/******发送断开连接*******/
		void SendKcpDisconnect(IUINT32 cov,KKAddress& addr,IUINT32 nTranId=0,IUINT32 nDestTranId=0);
		void SendKckLost(IUINT32 cov,KKAddress& addr,IUINT32 nTranId=0,IUINT32 nDestTranId=0);

		void SendSyn(IUINT32 cov,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		void SendSynAck(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		void SendAck(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);

		void SendLink(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		void SendUnLink(KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		///发送心跳
		void SendKcpHeart(IUINT32 cov,unsigned char HeartTick,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		/**********得到自身的转发Id************/
		void SendGetTranId(KKAddress& addr);
		//重新打包头。
		void SendSign(char* bufref,unsigned short &OutLen,IUINT32 &nTranId,IUINT32 &nDestTranId,KKAddress& addr);
private:
        volatile int             m_PreOk;
	    bool                     m_AutomaticHeart;
		kkBase::ckksocket        m_kksocket;
		kkBase::CKKMutex         m_UdpLock;
		/*******用于最后一次链接Id***********/
		IUINT32   m_conId;
		IUINT32   m_CurConv;
		/*******************IP信息*********************************/
		/************IpStr********iconv******/
		TKKTwoKeyMap<std::string,IUINT32,KcpInfo*>        m_ConIPInfoMap;

		//
		std::map<std::string,IUINT32>                      m_TranspondMap;

		/*******Udp发送数据**********/
		std::queue<KK_Cmd_Info*>                           m_SendUdpQue;
		kkBase::CKKMutex                                   m_SendUdpQueLock;
		
		/**********报队列接收的数据*****************/
		std::queue<KK_Packet*>                             m_RecPacketQue;
		kkBase::CKKMutex                                   m_RecPacketQueLock;

		kkBase::CKKMutex                                   m_KcpLock;
		/******************************/
		/**********接收区，缓存区*****************/
		char *m_pRecKcpbuf;
		///接收线程
		KK_THREAD_INFO                                     m_RecThread_info;
		///命令线程，主要处理会话握手
		KK_THREAD_INFO                                     m_CmdThread_info;
		///连接管理线程
		KK_THREAD_INFO                                     m_ConnMgrThread_info;
		bool                                               m_bRecWait;
};
#endif