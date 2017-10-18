#ifndef kkkcp_h_
#define kkkcp_h_
#include "ikcp.h"
#include "kkRes/socket/kkAddr.h"

#include "kkRes/Base/KKMutex.h"
#include "kkRes/Base/KKObj_Ref.h"
//0xAF    点对点通讯控制头
//0xAE    KCP消息头
//0xAD    转发消息
//0xAC    转发服务器通讯头
//0xAB    ///直接使用UDP发送数据
/********IP地址信息*********/
#define   KCP_MAXPKT_LEN   2000

//链接状态
enum KKConState
{
	KKUnKown=0,
	KKUnLink,
	KKSYN,            //发起者
	KKSYNACK,
	KKACK,            //发起者
	KKLink,           //链接已建立
	KKAutoDisConnect,
	KKDisConnect,     //断开链接
	KKKckLost,        //失去链接
	KKKcpHeart,       //kcp链接已经后的心跳包
	KKTranServer,     //转发服务器状态
	KKGetTranId,      //得到转发Id
	KKTranIdOk,       //转发Id建立Ok
	
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
	MSGTranHeart=11,      //转发服务器心跳
	MSGTranIdOk=12,       //转发Id建立Ok
	MSGUdpData=13,        //原生UDP数据
};
enum UDPMSG{
	UDPCTLUn=0,
	/********获取转发服务器上对于的Id*******/
	UDPGETTRANID=1,
	UDPDELTRANID,
	/***********未登记自身转发Id***************/
	UDPUNTRANID,
	/*********未定义转发Id********/
	UDPUNDEFTRANID,
	
	/************查询转发状态***************/
	UDPLOOKPRETRAN,
	UDPTranHeart,
	UDPCTLMSG=0xAF,
	
	
};
/****链接信息***/
class KcpInfo:public TObjRefImpl<IObjRef>
{
     public:
		        KcpInfo();
				
				void* operator new(size_t size);
				void  operator delete( void *ptr);
				/******************/
				IUINT32               Conv;          
				ikcpcb               *kcp;
				/****实际IP地址****/
				KKAddress            Addr; 

				//链接状态
				KKConState           State;
				unsigned short       mtu;
				//计时器
				unsigned long long   sendtick;///发送更新计数器。
				//心跳超时计数器
				unsigned long long   rectick;
				/********实际发送者对象**********/
				void                 *Send;        
				/******发起者******/ 
				bool                  sponsor;     
				/***握手次数********/
				unsigned char         HandShake;   
				/******是否转发****/
				bool                  bTranspond; 

				/******转发自身Id******/
				IUINT32                nTranId;
				/******转发Id******/
				IUINT32                nDestTranId;

				unsigned int           PackId;///分包ID kcp传输大于2000的数据包会有问题
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
/******转发信息*********/
typedef struct KKTraInfo
{
    IUINT32 nTranId;
}KKTraInfo;


enum KKCMD{
	CMDUnKown,
	CMDConnect,
	CMDDisConnect,
};
/*******CMD信息*******/
typedef struct KK_Cmd_Info{
  unsigned char *buf;
  unsigned short len;
  IUINT32        Conv;   
  IUINT32        nTranId;
  IUINT32        nDestTranId;
  /****实际IP地址****/
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

///生成内存
void          *KKmalloc(size_t _Size);
void           KKfree(void * _Memory);
///释放内存
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
///简单的效验
unsigned int   kk_check_sum(unsigned char * data,unsigned int  DataSize);
#endif