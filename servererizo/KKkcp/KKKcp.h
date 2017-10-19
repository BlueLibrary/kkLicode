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
		/*****ת����******/
		int SetTracker(std::string Ip,int port);
		/****��Ip***/
		int KkBind(const char *Ip,unsigned short port);
		
        //// ��ʽ�ӿ���Ҫ���������˲��ܷ���
        int                    StablySend(IUINT32 Id,const char* buf,int buflen);

		///ConId���ͻ�����Ҫ�����˲��ܷ��͡�
		int                    UdpSendByConId(IUINT32 ConId,const char* buf,int buflen);
		int                    UdpSendByAddr(KKAddress& Addr,const char* buf,int buflen);
		unsigned short         GetMtuSize(IUINT32 ConId);
		/********����Զ��**********/
		int Connect(std::string Ip,int port);
		//ͨ��ת������������Ŀ�Ķ�
		int Connect(std::string TranIp,kk_ushort port,kk_ulong nDestTranId);
		/*******����ת��������*********/
		int ConnectTranServer(std::string Ip,kk_ushort port);
		
		/********�õ�һ��ת��������TranId*********/
		IUINT32 GetOwerTranId(std::string Ip,int port);
		//�Ͽ�����
		int DisConnect(IUINT32 Id);
		//�Ͽ�����
		int DisConnect(std::string Ip,int port,kk_ulong nDestTranId);
		/********��ȡ���յ����ݰ�*********/
		KK_Packet* Receive();
		void DelPacket(KK_Packet* packet);
private:
/********ʹ��kcp�������ݣ�����ʧ�ܷ���-1��buflen ���ֵ2000*********/
		int KKSend(IUINT32 Id,const char* buf,unsigned short buflen);
		IUINT32        m_nLocalTranId;
		///���������̣߳��Ƿ��Ǻ����������̺߳ϲ�
		static unsigned __stdcall CmdThread(void *pUserData);
		void HandleCmd();	

		///Udp�������ݴ����߳�
		static unsigned __stdcall RecUdpThread(void *pUserData);
		void HandleRecUdp();

		///���ӹ����߳�
		static unsigned __stdcall ConnMgrThread(void *pUserData);
		void ConnMgrThread2();

		void OnConHandle(unsigned char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr); 
		void OnKcp(unsigned char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr); 
		ikcpcb* CreateKcp(std::string Ipport,IUINT32 cov,unsigned short mtu);//ikcpcb *kcp = ikcp_create(conv, user);

		void OnBufHandle(char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr);
		void OnTranBufHandle(unsigned char* buf,unsigned short buflen,KKAddress &addr);
		/******��һ�����յ����ݰ�*******/
		void PutKKPacket(KK_Packet* packet);
		void GetRecKcpData(ikcpcb *kcp,KKAddress& Addr);
		void GenSysKKPacket(KKConState sys,KKAddress& Addr,unsigned long conId);
		/************�ͷ�*************/
		void FreeKcpInfo(std::map<std::string,KcpInfo* >::iterator &KcpIt,bool NeedMsg=true);

		/******���ͶϿ�����*******/
		void SendKcpDisconnect(IUINT32 cov,KKAddress& addr,IUINT32 nTranId=0,IUINT32 nDestTranId=0);
		void SendKckLost(IUINT32 cov,KKAddress& addr,IUINT32 nTranId=0,IUINT32 nDestTranId=0);

		void SendSyn(IUINT32 cov,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		void SendSynAck(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		void SendAck(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);

		void SendLink(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		void SendUnLink(KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		///��������
		void SendKcpHeart(IUINT32 cov,unsigned char HeartTick,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId);
		/**********�õ������ת��Id************/
		void SendGetTranId(KKAddress& addr);
		//���´��ͷ��
		void SendSign(char* bufref,unsigned short &OutLen,IUINT32 &nTranId,IUINT32 &nDestTranId,KKAddress& addr);
private:
        volatile int             m_PreOk;
	    bool                     m_AutomaticHeart;
		kkBase::ckksocket        m_kksocket;
		kkBase::CKKMutex         m_UdpLock;
		/*******�������һ������Id***********/
		IUINT32   m_conId;
		IUINT32   m_CurConv;
		/*******************IP��Ϣ*********************************/
		/************IpStr********iconv******/
		TKKTwoKeyMap<std::string,IUINT32,KcpInfo*>        m_ConIPInfoMap;

		//
		std::map<std::string,IUINT32>                      m_TranspondMap;

		/*******Udp��������**********/
		std::queue<KK_Cmd_Info*>                           m_SendUdpQue;
		kkBase::CKKMutex                                   m_SendUdpQueLock;
		
		/**********�����н��յ�����*****************/
		std::queue<KK_Packet*>                             m_RecPacketQue;
		kkBase::CKKMutex                                   m_RecPacketQueLock;

		kkBase::CKKMutex                                   m_KcpLock;
		/******************************/
		/**********��������������*****************/
		char *m_pRecKcpbuf;
		///�����߳�
		KK_THREAD_INFO                                     m_RecThread_info;
		///�����̣߳���Ҫ����Ự����
		KK_THREAD_INFO                                     m_CmdThread_info;
		///���ӹ����߳�
		KK_THREAD_INFO                                     m_ConnMgrThread_info;
		bool                                               m_bRecWait;
};
#endif