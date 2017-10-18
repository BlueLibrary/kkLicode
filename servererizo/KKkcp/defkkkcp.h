#ifndef kkkcp_h_
#define kkkcp_h_
#include "ikcp.h"
#include "kkRes/socket/kkAddr.h"

#include "kkRes/Base/KKMutex.h"
#include "kkRes/Base/KKObj_Ref.h"
//0xAF    ��Ե�ͨѶ����ͷ
//0xAE    KCP��Ϣͷ
//0xAD    ת����Ϣ
//0xAC    ת��������ͨѶͷ
//0xAB    ///ֱ��ʹ��UDP��������
/********IP��ַ��Ϣ*********/
#define   KCP_MAXPKT_LEN   2000

//����״̬
enum KKConState
{
	KKUnKown=0,
	KKUnLink,
	KKSYN,            //������
	KKSYNACK,
	KKACK,            //������
	KKLink,           //�����ѽ���
	KKAutoDisConnect,
	KKDisConnect,     //�Ͽ�����
	KKKckLost,        //ʧȥ����
	KKKcpHeart,       //kcp�����Ѿ����������
	KKTranServer,     //ת��������״̬
	KKGetTranId,      //�õ�ת��Id
	KKTranIdOk,       //ת��Id����Ok
	
};
enum KKP2PMSGID{
	MSGUnKown=0,
	MSGUnLink=1,
	MSGKckLost=2,
    MSGConnectFail=3,
	MSGLinkLost=4,
	MSGLinkAccepted=5,
    MSGLinkIncoming=6,
	MSGKcpHeart=7,
	MSGKcpData=8,
    MSGKcpDisConnect=9,
    MSGTranConnectFail=10,	
	MSGTranHeart=11,      //ת������������
	MSGTranIdOk=12,       //ת��Id����Ok
	MSGUdpData=13,        //ԭ��UDP����
};
enum UDPMSG{
	UDPCTLUn=0,
	/********��ȡת���������϶��ڵ�Id*******/
	UDPGETTRANID=1,
	UDPDELTRANID,
	/***********δ�Ǽ�����ת��Id***************/
	UDPUNTRANID,
	/*********δ����ת��Id********/
	UDPUNDEFTRANID,
	
	/************��ѯת��״̬***************/
	UDPLOOKPRETRAN,
	UDPTranHeart,
	UDPCTLMSG=0xAF,
	
	
};
/****������Ϣ***/
class KcpInfo:public TObjRefImpl<IObjRef>
{
     public:
		        KcpInfo();
				
				void* operator new(size_t size);
				void  operator delete( void *ptr);
				/******************/
				IUINT32               Conv;          
				ikcpcb               *kcp;
				/****ʵ��IP��ַ****/
				KKAddress            Addr; 

				//����״̬
				KKConState           State;
				unsigned short       mtu;
				//��ʱ��
				unsigned long long   sendtick;///���͸��¼�������
				//������ʱ������
				unsigned long long   rectick;
				/********ʵ�ʷ����߶���**********/
				void                 *Send;        
				/******������******/ 
				bool                  sponsor;     
				/***���ִ���********/
				unsigned char         HandShake;   
				/******�Ƿ�ת��****/
				bool                  bTranspond; 

				/******ת������Id******/
				IUINT32                nTranId;
				/******ת��Id******/
				IUINT32                nDestTranId;

				unsigned int           PackId;///�ְ�ID kcp�������2000�����ݰ���������
private:
	~KcpInfo();
};
class CKcpGurd
{
public:
	CKcpGurd(KcpInfo& KcpObj);
	~CKcpGurd();
private:
     KcpInfo& KcpInfoObj;
};
/******ת����Ϣ*********/
typedef struct KKTraInfo
{
    IUINT32 nTranId;
}KKTraInfo;


enum KKCMD{
	CMDUnKown,
	CMDConnect,
	CMDDisConnect,
};
/*******CMD��Ϣ*******/
typedef struct KK_Cmd_Info{
  unsigned char *buf;
  unsigned short len;
  IUINT32        Conv;   
  IUINT32        nTranId;
  IUINT32        nDestTranId;
  /****ʵ��IP��ַ****/
  KKAddress      Addr; 
  KKCMD cmd;
  KK_Cmd_Info();

  void* operator new(size_t size);
  void operator delete( void *ptr);
}KK_Cmd_Info;

typedef struct KK_Packet{
	unsigned char *buf;
	int len;
	KKP2PMSGID msgId;
    KKAddress Addr;
	IUINT32 ConId;
    KK_Packet();
	void* operator new(size_t size);
	void operator delete( void *ptr);
}KK_Packet;

void KKSleep(int mi);
unsigned long long   KKTickCount();

///�����ڴ�
void          *KKmalloc(size_t _Size);
void           KKfree(void * _Memory);
///�ͷ��ڴ�
unsigned char* putsyn(unsigned long cov,unsigned short &OutLen,unsigned int nDestTranId);
unsigned char* putsynack(unsigned long cov,unsigned short mtu,unsigned short &OutLen);
unsigned char* putack(unsigned long cov,unsigned short mtu,unsigned short &OutLen);
unsigned char* putlink(unsigned long cov,unsigned short mtu,unsigned short &OutLen);
unsigned char* putkcpheart(unsigned long cov,unsigned short &OutLen);
unsigned char* putkcpdisconnect(unsigned long cov,unsigned short &OutLen);
unsigned char* putunlink(unsigned short &OutLen);
unsigned char* putkcplost(unsigned long cov,unsigned short &OutLen);
unsigned char* putGetTranId(unsigned short &OutLen);
unsigned char* putDelTranId(unsigned short &OutLen);
unsigned char* puttranheart(unsigned short &OutLen);
int            butTranspondHeadr(unsigned char* Buf,unsigned short bufLen,IUINT32 nTranId,IUINT32 nDestTranId);
///�򵥵�Ч��
unsigned int   kk_check_sum(unsigned char * data,unsigned int  DataSize);
#endif