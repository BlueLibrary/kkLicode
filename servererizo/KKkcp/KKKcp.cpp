#include "KKKcp.h"
#include <assert.h>
#define MAXBUF            1048576   // 1mb
#define MinHeartValue     5000      // 5s
#define MaxHeartValue     60000     //一分钟。	
#define SYNTICK           5000
#define SYNTICKD          10000
#define SYNTICKT          20000
#define MAXHANDSHAKE      4
#define ACKTICK           5000
#define MaxConTick        60000 //一分钟
#define MaxReSendCount    10
#define ConPrintLg 1


int CKKkcp::GetIpV4ByHostName(const char *host_name,std::string &OutIp)
{
 return ::GetIpV4ByHostName(host_name,OutIp);
}

CKKkcp::CKKkcp():m_PreOk(0),m_CurConv(100),m_conId(0),m_pRecKcpbuf(NULL),m_AutomaticHeart(false)
{
	int lxx=sizeof(KcpInfo);
	m_pRecKcpbuf=(char*)KKmalloc(MAXBUF);
	memset(&m_RecThread_info,0,sizeof(KK_THREAD_INFO));
	memset(&m_CmdThread_info,0,sizeof(KK_THREAD_INFO));

	m_RecThread_info.ThreadExit=true;
	m_CmdThread_info.ThreadExit=true;
	m_ConnMgrThread_info.ThreadExit=true;
	m_nLocalTranId=0;
}
CKKkcp::~CKKkcp()
{
	//return;
	m_PreOk=2;
	if(m_PreOk==0||m_PreOk==2){
		while(1){
			if(m_RecThread_info.ThreadExit==true&&m_CmdThread_info.ThreadExit==true&&m_ConnMgrThread_info.ThreadExit==true){
				break;
			}
			KKSleep(10);
		}
	}

	unsigned short OutLen=0;
	char * DisBuf=(char * )putkcpdisconnect(0,OutLen);

	m_ConIPInfoMap.lock(false);
	std::map<std::string,KcpInfo* >::iterator KcpIt=m_ConIPInfoMap.Key1begin();
	for(;KcpIt!=m_ConIPInfoMap.Key1End();){

		KcpInfo* info=KcpIt->second;
		info->AddRef();
		if(info->sponsor&&info->State==KKLink&&info->kcp!=NULL){
			
			m_kksocket.kksend(DisBuf,OutLen,info->Addr);
		}
		FreeKcpInfo(KcpIt,false);
		info->Release();
	}
	m_ConIPInfoMap.unlock(false);
	KKfree(DisBuf);
	KKfree(m_pRecKcpbuf);
}
int  CKKkcp::KkBind(const char *Ip,unsigned short port)
{
	return m_kksocket.kkbind(Ip,port);
}
void CKKkcp::Start(bool AutomaticHeart)
{
	if(m_PreOk==0)
	{
		    m_PreOk=1;
			m_AutomaticHeart=AutomaticHeart;
			#ifdef WIN32
					m_RecThread_info.ThreadHandle=(HANDLE)_beginthreadex(NULL, NULL,RecUdpThread, (LPVOID)this, 0,&m_RecThread_info.ThreadAddr);
					m_CmdThread_info.ThreadHandle=(HANDLE)_beginthreadex(NULL, NULL,CmdThread, (LPVOID)this, 0,&m_CmdThread_info.ThreadAddr);
					m_ConnMgrThread_info.ThreadHandle=(HANDLE)_beginthreadex(NULL, NULL,ConnMgrThread, (LPVOID)this, 0,&m_ConnMgrThread_info.ThreadAddr);
			#else
					m_RecThread_info.ThreadAddr     =  pthread_create(&m_RecThread_info.Tid_task, NULL, (void* (*)(void*))RecUdpThread, (void*)this);
					m_CmdThread_info.ThreadAddr     =  pthread_create(&m_CmdThread_info.Tid_task, NULL, (void* (*)(void*))CmdThread, (void*)this);
					m_ConnMgrThread_info.ThreadAddr =  pthread_create(&m_ConnMgrThread_info.Tid_task, NULL, (void* (*)(void*))ConnMgrThread, (void*)this);
			#endif
			
	}
}

/*****打洞, 转发者******/
int CKKkcp::SetTracker(std::string Ip,int port)
{
	
	/*MAKEWORD*/
	return 0;
}

int CKKkcp::StablySend(IUINT32 Id,const char* buf,int buflen)
 {
     int count=buflen/KCP_MAXPKT_LEN;
	 char* tempbuf=(char*) buf;

	int Ret=0;
	m_ConIPInfoMap.lock(true);
	if(Id==0){
		std::map<IUINT32,KcpInfo*>::iterator It2=m_ConIPInfoMap.Key2begin();
		if(It2!=m_ConIPInfoMap.Key2End())
		{
			m_conId=It2->first;
			Id=m_conId;
		}
	
	}

	KcpInfo* kcpInfo=NULL;
	std::map<IUINT32,KcpInfo* >::iterator It=m_ConIPInfoMap.Key2Find(Id);
	if(It!=m_ConIPInfoMap.Key2End()){
		 kcpInfo=It->second;
		 kcpInfo->AddRef();
	}else{
		Ret=-1;
	}
	m_ConIPInfoMap.unlock(true);
	
	if(kcpInfo!=NULL)
	{
		if(kcpInfo->sponsor){
	       kcpInfo->sendtick=KKTickCount();
		}else{
		    kcpInfo->rectick=KKTickCount();
		} 
		for(int i=0;i<count;++i){
			  Ret=ikcp_send(kcpInfo->kcp,tempbuf,KCP_MAXPKT_LEN);
			  tempbuf    +=KCP_MAXPKT_LEN;
			  buflen -=KCP_MAXPKT_LEN;
		 }
		 if(buflen>0)
		 {
			  Ret=ikcp_send(kcpInfo->kcp,tempbuf,buflen);
		 }
		
		if(Ret<0){
			assert(0);
		  //  kcpInfo->State=KKKckLost;
		}
		kcpInfo->Release();
		return buflen;
	}
	return 0;
 }
int CKKkcp::KKSend(IUINT32 Id,const char* buf,unsigned short buflen)
{
	int Ret=0;
	static std::map<IUINT32,KcpInfo* >::iterator It;
	m_ConIPInfoMap.lock(true);
	if(Id==0){
		std::map<IUINT32,KcpInfo*>::iterator It2=m_ConIPInfoMap.Key2begin();
		if(It2!=m_ConIPInfoMap.Key2End())
		{
			m_conId=It2->first;
			Id=m_conId;
		}
	
	}

	KcpInfo* kcpInfo=NULL;
	It=m_ConIPInfoMap.Key2Find(Id);
	if(It!=m_ConIPInfoMap.Key2End()){
		 kcpInfo=It->second;
		 kcpInfo->AddRef();
	}else{
		Ret=-1;
	}
	m_ConIPInfoMap.unlock(true);
	
	if(kcpInfo!=NULL)
	{
		if(kcpInfo->sponsor){
	       kcpInfo->sendtick=KKTickCount();
		}else{
		    kcpInfo->rectick=KKTickCount();
		}
		Ret=ikcp_send(kcpInfo->kcp, buf, buflen);
		if(Ret<0){
			assert(0);
		    kcpInfo->State=KKKckLost;
		}
		kcpInfo->Release();
	}
	return Ret;
}

int CKKkcp::UdpSendByConId(IUINT32 ConId,const char* buf,int buflen)
{
	int ret=0;
	m_ConIPInfoMap.lock(false);
	std::map<IUINT32,KcpInfo*>::iterator It=m_ConIPInfoMap.Key2Find(ConId);
	if(It!=m_ConIPInfoMap.Key2End()){
		
	}else{
		ret=-1;
	}
	m_ConIPInfoMap.unlock(false);
	return ret;
}
int CKKkcp::UdpSendByAddr(KKAddress& Addr,const char* buf,int buflen)
{
	int ret=-1;
	return ret;
}
unsigned short CKKkcp::GetMtuSize(IUINT32 ConId)
{
	unsigned short mtu_size=0;
	m_ConIPInfoMap.lock(false);
	std::map<IUINT32,KcpInfo*>::iterator It=m_ConIPInfoMap.Key2Find(ConId);
	if(It!=m_ConIPInfoMap.Key2End()){
		It->second->AddRef();
		mtu_size=It->second->mtu;
		It->second->Release();
	}
	m_ConIPInfoMap.unlock(false);
	return mtu_size;
}
int CKKkcp::Connect(std::string ip,int port)
{
	unsigned int Ret=0;
	char PortStr[64]="";
	sprintf(PortStr,"|%d",port);
	std::string Ipstr=ip;
	Ipstr+=PortStr;

	
	std::map<std::string,KcpInfo*>::iterator It;
	KcpInfo* info =NULL;
	m_ConIPInfoMap.lock(false);
	It=m_ConIPInfoMap.Key1Find(Ipstr);
	if(It!=m_ConIPInfoMap.Key1End()){
		It->second->AddRef();
		if(It->second->State<KKSYN){
			assert(0);
		}
		It->second->Release();
	}else{
			KcpInfo* info = new KcpInfo();
			info->Send=&m_kksocket;
			info->Addr.FromString(ip.c_str(),port);
			info->kcp=NULL;
			info->Conv=0;
			info->sponsor=true;
			info->State=KKSYN;
			m_CurConv++;
			info->Conv=m_CurConv;
			m_ConIPInfoMap.Key1Insert(Ipstr,info);

			KK_Cmd_Info* udp_info =new KK_Cmd_Info();
			udp_info->buf=NULL;
			udp_info->len=0;
			udp_info->nDestTranId=0;
			udp_info->nTranId=0;
			udp_info->Conv=info->Conv;
			udp_info->cmd=CMDConnect;
			udp_info->Addr=info->Addr;
			
			m_SendUdpQueLock.Lock();
			m_SendUdpQue.push(udp_info);
			m_SendUdpQueLock.UnLock();
	}
	m_ConIPInfoMap.unlock(false);
	return 0;
}

//通过转发服务器连接目的端
int CKKkcp::Connect(std::string TranIp,kk_ushort port,kk_ulong nDestTranId)
{
	int Ret=0;
	char IPPortStr[64]="";

	sprintf(IPPortStr,"%s|%d",TranIp.c_str(),port);
	

	std::map<std::string,KcpInfo*>::iterator It;
	std::map<std::string,KcpInfo*>::iterator It2;
	KcpInfo* info =NULL;
	m_ConIPInfoMap.lock(false);
	It=m_ConIPInfoMap.Key1Find(IPPortStr);
	if(It!=m_ConIPInfoMap.Key1End()){
		sprintf(IPPortStr,"%s|%d|%d",TranIp.c_str(),port,nDestTranId);
		It2=m_ConIPInfoMap.Key1Find(IPPortStr);
		if(It2==m_ConIPInfoMap.Key1End()){
				KcpInfo* info = new KcpInfo();
				info->Send=&m_kksocket;
				info->Addr.FromString(TranIp.c_str(),port,nDestTranId);
				info->kcp=NULL;
				info->Conv=0;
				info->bTranspond=true;
				info->sponsor=true;
				info->State=KKSYN;
				info->nTranId=It->second->nTranId;
				info->nDestTranId=nDestTranId;
				info->Addr.ToString(true,IPPortStr,'|');
				m_ConIPInfoMap.Key1Insert(IPPortStr,info);
				m_CurConv++;
				info->Conv=m_CurConv;

				KK_Cmd_Info* udp_info = new KK_Cmd_Info();
				udp_info->cmd=CMDConnect;
				udp_info->Addr=info->Addr;
				udp_info->nTranId=info->nTranId;
				udp_info->nDestTranId=info->nDestTranId;
				udp_info->Conv=info->Conv;
				udp_info->buf=NULL;
				udp_info->len=0;

				m_SendUdpQueLock.Lock();
				m_SendUdpQue.push(udp_info);
				m_SendUdpQueLock.UnLock();
		}else{
			Ret=-1;
		}
	}
	m_ConIPInfoMap.unlock(false);
	return 0;
}
int CKKkcp::ConnectTranServer(std::string Ip,kk_ushort port)
{
	unsigned int Ret=0;
	char PortStr[64]="";
	sprintf(PortStr,"|%d",port);
	std::string Ipstr=Ip;
	Ipstr+=PortStr;

	KcpInfo* info=NULL;
	std::map<std::string,KcpInfo*>::iterator It;
	m_ConIPInfoMap.lock(false);
	It=m_ConIPInfoMap.Key1Find(Ipstr);
	if(It!=m_ConIPInfoMap.Key1End()){
			assert(0);
	}else{
		info = new KcpInfo();
		info->Send=&m_kksocket;
		info->Addr.FromString(Ip.c_str(),port);
		info->kcp=NULL;
		info->Conv=0;
		info->sponsor=true;
		info->State=KKGetTranId;
		
		info->bTranspond=true;
		info->nDestTranId=0;
		info->nTranId=0;
		m_ConIPInfoMap.Key1Insert(Ipstr,info);
	}
	m_ConIPInfoMap.unlock(false);
	if(info==NULL){
		return -1;
	}
	
	return 0;
}
IUINT32 CKKkcp::GetOwerTranId(std::string Ip,int port)
{

	return 0;
}
int CKKkcp::DisConnect(IUINT32 Id)
{
	int ret=0;
	m_ConIPInfoMap.lock(false);
	std::map<IUINT32,KcpInfo*>::iterator It=m_ConIPInfoMap.Key2Find(Id);
	if(It!=m_ConIPInfoMap.Key2End()){
		KK_Cmd_Info* udp_info = new KK_Cmd_Info();
		udp_info->cmd=CMDDisConnect;
		udp_info->Addr=It->second->Addr;
		udp_info->Conv=It->second->Conv;
		udp_info->nDestTranId=It->second->nDestTranId;
		udp_info->nTranId=It->second->nTranId;
		udp_info->buf=NULL;
		udp_info->len=NULL;
		
		m_SendUdpQueLock.Lock();
		m_SendUdpQue.push(udp_info);
		m_SendUdpQueLock.UnLock();
	}else{
		ret=-1;
	}
	m_ConIPInfoMap.unlock(false);
	return ret;
}
int CKKkcp::DisConnect(std::string Ip,int port,kk_ulong nDestTranId)
{
	char abcd[256]="";
	if(nDestTranId>0){
		sprintf(abcd,"%s|%d|%d",Ip.c_str(),port,nDestTranId);
	}else{
		sprintf(abcd,"%s|%d",Ip.c_str(),port);
	}
	
	m_ConIPInfoMap.lock(false);
	std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(abcd);
	if(It!=m_ConIPInfoMap.Key1End()){
		It->second->AddRef();
		KK_Cmd_Info* udp_info =new KK_Cmd_Info();
		udp_info->cmd=CMDDisConnect;
		udp_info->Addr=It->second->Addr;
		udp_info->Conv=It->second->Conv;
		udp_info->nDestTranId=It->second->nDestTranId;
		udp_info->nTranId=It->second->nTranId;
		udp_info->buf=NULL;
		udp_info->len=NULL;

	    It->second->State=KKAutoDisConnect;
		It->second->sendtick=::KKTickCount();
		It->second->Release();
		m_SendUdpQueLock.Lock();
		m_SendUdpQue.push(udp_info);
		m_SendUdpQueLock.UnLock();
	}
	m_ConIPInfoMap.unlock(false);
	return 0;
}
//发送数据
int kk_udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
	
	KcpInfo *info=(KcpInfo *)user;
	if(info!=0&&len>0)
	{
		if(len>2048)
			assert(0);
		info->AddRef();
		kkBase::ckksocket* psocket=(kkBase::ckksocket*)info->Send;
		if(info->bTranspond){
			butTranspondHeadr(psocket->m_pTemp,1024,info->nTranId,info->nDestTranId);
			*(psocket->m_pTemp+9)=0xAE;
			memmove((psocket->m_pTemp+10),buf,len);	
			len+=10;
		}else{
			*psocket->m_pTemp=0xAE;
			memmove((psocket->m_pTemp+1),buf,len++);
		}
		info->Release();
		return psocket->kksend((char*)psocket->m_pTemp,len,info->Addr);
	}
	return 0;
}
unsigned __stdcall CKKkcp:: CmdThread(void *pUserData)
{
	CKKkcp *pIns=(CKKkcp *)pUserData;
	pIns->m_CmdThread_info.ThreadExit=false;
	pIns->HandleCmd();
	return 0;
}
void CKKkcp::HandleCmd()
{
	while(m_PreOk==1){
		KK_Cmd_Info *pInfo=NULL;
		m_SendUdpQueLock.Lock();
		if(m_SendUdpQue.size()>0){
			pInfo=m_SendUdpQue.front();
			m_SendUdpQue.pop();
		}
		m_SendUdpQueLock.UnLock();

		if(pInfo!=NULL){
			if(pInfo->cmd==CMDConnect){
				SendSyn(pInfo->Conv,pInfo->Addr,pInfo->nTranId,pInfo->nDestTranId);
			}else if(pInfo->cmd==CMDDisConnect){
				SendKcpDisconnect(pInfo->Conv,pInfo->Addr,pInfo->nTranId,pInfo->nDestTranId);
			} 
			if(pInfo->buf!=NULL)
		       KKfree(pInfo->buf);
		   delete pInfo;
		}else{
            KKSleep(10); 
		}
	}
    m_CmdThread_info.ThreadExit=true;
}

/********Udp数据接收***********/
unsigned __stdcall CKKkcp::RecUdpThread(void *pUserData){
	CKKkcp *pIns=(CKKkcp *)pUserData;
	pIns->m_RecThread_info.ThreadExit=false;
	pIns->HandleRecUdp();
	return 0;
}
/*************处理接收数据*************/
void CKKkcp::HandleRecUdp()
{
	unsigned short bufLen=2048, OutLen=0;
	KKAddress adr;
	int RecLen=0;
	char *RecBuf=(char*)KKmalloc(bufLen);
	while(m_PreOk==1)
	{
		RecLen=m_kksocket.kkrecvfrom(RecBuf,bufLen,adr);
		if(RecLen>0){
			   //处理接收的数据
			   OnBufHandle(RecBuf,RecLen,0,0,adr);
			   OutLen=0;
		}else {
			if(OutLen>512)
			{
			    OutLen=0;
				KKSleep(5);
			}else
			++OutLen;
		  //    
		}
	}
	KKfree(RecBuf);
	m_RecThread_info.ThreadExit=true;
}

unsigned __stdcall CKKkcp::ConnMgrThread(void *pUserData)
{
	CKKkcp *pIns=(CKKkcp *)pUserData;
	pIns->m_ConnMgrThread_info.ThreadExit=false;
	pIns->ConnMgrThread2();
    return 0;
}
void CKKkcp::ConnMgrThread2()
{
	unsigned long long millisec=0,kcptick=0,rectick=0;
	unsigned long  conv=0;
	unsigned char typex=0;
	
	KcpInfo* pKcpInfo=NULL;
	std::map<std::string,KcpInfo* >::iterator KcpIt;
	while(m_PreOk==1)
	{
			millisec=KKTickCount();
			m_ConIPInfoMap.lock(false);
			KcpIt=m_ConIPInfoMap.Key1begin();
			for(;KcpIt!=m_ConIPInfoMap.Key1End();)
			{
				pKcpInfo=KcpIt->second;
				CKcpGurd grd(*pKcpInfo);
				kcptick=millisec>pKcpInfo->sendtick ? millisec-pKcpInfo->sendtick:0;
				rectick=millisec>pKcpInfo->rectick?millisec-pKcpInfo->rectick:0;
				if(pKcpInfo->kcp!=NULL){
					///检查kcp
					m_KcpLock.Lock();
					unsigned int ic=millisec& 0xfffffffful;
					if(ikcp_check(pKcpInfo->kcp,ic))
					{
						ikcp_update(pKcpInfo->kcp,ic);
						GetRecKcpData(pKcpInfo->kcp,pKcpInfo->Addr);
					}
					m_KcpLock.UnLock();
				}

				if(pKcpInfo->State==KKAutoDisConnect||pKcpInfo->State==KKKckLost||!pKcpInfo->sponsor&&kcptick>MaxHeartValue)
				{
					FreeKcpInfo(KcpIt);
					continue;
				}else
				{
					//与转发服务器建立连接
					if(pKcpInfo->State==KKGetTranId)
					{
						if(pKcpInfo->HandShake==0&&kcptick>SYNTICKD||pKcpInfo->HandShake==1&&kcptick>SYNTICKT||pKcpInfo->HandShake==2&&kcptick>MaxConTick)
						{
								pKcpInfo->HandShake++;
								if(pKcpInfo->HandShake<3){
									pKcpInfo->rectick=pKcpInfo->sendtick=millisec;
									SendGetTranId(pKcpInfo->Addr);
								}	
						}else if(pKcpInfo->HandShake>2){
								/****失败*****/
								pKcpInfo->HandShake=0;
								pKcpInfo->rectick=pKcpInfo->sendtick=millisec;
								FreeKcpInfo(KcpIt,true);
								continue;
						}
					}else if(pKcpInfo->sponsor){
						/************还未建立连接*****************/
						if(pKcpInfo->HandShake < MAXHANDSHAKE &&(pKcpInfo->State==KKSYN || pKcpInfo->State==KKACK))
						{
								//连接超时
								if(kcptick>MaxConTick){
									FreeKcpInfo(KcpIt);
									continue;
								}else if(pKcpInfo->State==KKSYN){
									if( pKcpInfo->HandShake==0&&kcptick>SYNTICK||pKcpInfo->HandShake==1&&kcptick>SYNTICKD){
										pKcpInfo->HandShake++;
										pKcpInfo->rectick=pKcpInfo->sendtick=millisec;
										SendSyn(pKcpInfo->Conv,pKcpInfo->Addr,pKcpInfo->nTranId,pKcpInfo->nDestTranId);
									}else if(pKcpInfo->HandShake>=2&&kcptick>SYNTICKT){//握手超时
										pKcpInfo->HandShake++;
										FreeKcpInfo(KcpIt);
										continue;
									}
								}else if(pKcpInfo->State==KKACK){
									if( pKcpInfo->HandShake==0&&kcptick>SYNTICK||pKcpInfo->HandShake==1&&kcptick>SYNTICKD){
										pKcpInfo->HandShake++;
										pKcpInfo->rectick=pKcpInfo->sendtick=millisec;
										SendAck(pKcpInfo->Conv,pKcpInfo->mtu,pKcpInfo->Addr,pKcpInfo->nTranId,pKcpInfo->nDestTranId);
									}else if(pKcpInfo->HandShake>=2&&kcptick>SYNTICKT){
										pKcpInfo->HandShake++;
										FreeKcpInfo(KcpIt);
										continue;
									}
								}
						}else if(m_AutomaticHeart&&pKcpInfo->State==KKLink&&kcptick>MinHeartValue){
							/************自动维护心跳************/
							pKcpInfo->sendtick=millisec;
							SendKcpHeart(pKcpInfo->Conv,0,pKcpInfo->Addr,pKcpInfo->nTranId,pKcpInfo->nDestTranId);
						}else if(rectick>MaxHeartValue){///没有收到服务器心跳，超时
#ifdef ConPrintLg
							printf("%d 服务器心跳超时 \n");
#endif
							FreeKcpInfo(KcpIt);
							continue;
					    }
					}else if(pKcpInfo->State==KKTranIdOk&&kcptick>MinHeartValue){
							/***********转发服务器************/
							unsigned char heartx[15]={0xAC,UDPTranHeart,0};
							pKcpInfo->sendtick=millisec;
							m_kksocket.kksend((const char*)heartx,15,KcpIt->second->Addr);
					}else if(!pKcpInfo->sponsor){///连接被动接受者，即服务端
						    if(pKcpInfo->State==KKLink&&rectick>MinHeartValue){
								    ///服务端需要定时向客户发送心跳
									unsigned char heartx[15]={0xAC,UDPTranHeart,0};
									pKcpInfo->rectick=millisec;
									m_kksocket.kksend((const char*)heartx,15,KcpIt->second->Addr);
							}
							
					       if(pKcpInfo->State==KKLink&&kcptick>MaxHeartValue)
						   {
#ifdef ConPrintLg
							   printf("%d 客户端心跳超时 \n");
#endif
							   FreeKcpInfo(KcpIt);
							   continue;
						   }
					}
				}
				++KcpIt;
			}
			m_ConIPInfoMap.unlock(false);
			KKSleep(5);
	}
	m_ConnMgrThread_info.ThreadExit=TRUE;
}

void CKKkcp::OnBufHandle(char* RecBuf,unsigned short RecLen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &adr)
{
	unsigned char  typex=*RecBuf;
	unsigned long  conv=0;
	unsigned long  conv2=0;
	//printf("\n Data: %x\n",typex);
	if(typex==0xAF){ 
		//点对点通讯控制头
		conv=ntohl(*(unsigned long*)(RecBuf+1)) ;
		switch(conv){
				case   UDPCTLMSG:
					           OnConHandle((unsigned char*)(RecBuf+5),RecLen-5,nTranId,nDestTranId,adr);
					           break;
		}
	}else if(typex==0xAE){
		/*******kcp消息******/
		OnKcp((unsigned char*)(RecBuf+1),RecLen-1,nTranId,nDestTranId,adr);
	}else if(typex==0xAD){
		/******转发消息********/
		/*****目的Id****/
		conv=ntohl(*(unsigned long*)(RecBuf+1));
		/******自身ID*****/
		conv2=ntohl(*(unsigned long*)(RecBuf+5));
		char* buf=(char*)(RecBuf+9);
		RecLen-=9;
		OnBufHandle(buf,RecLen,conv,conv2,adr);
	}else if(typex==0xAC){
		//转发服务器通讯头
		/*******转发服务器通讯头*********/
		OnTranBufHandle((unsigned char*)(RecBuf+1),RecLen-1,adr);
	}else if(typex==0xAB&&RecLen>1){
	     ///原生UDP消息
		KK_Packet* packet= new KK_Packet();
		packet->Addr=adr;
		packet->msgId=MSGUdpData;
		packet->ConId=0;
		packet->len=RecLen-1;
		packet->buf=(unsigned char* )::KKmalloc(packet->len);
		memmove(packet->buf,(unsigned char*)(RecBuf+1),packet->len);
		PutKKPacket(packet);
	}
}
void CKKkcp::OnConHandle(unsigned char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr)
{
	static int packetlen=sizeof(KK_Packet);
	char ipport[256]="";
	unsigned long msgId=ntohl(*(unsigned long*)buf);
	if(msgId==KKDisConnect)
	{
#ifdef ConPrintLg
		printf("\n  客户端断开连接 KKAutoDisConnect\n");
#endif
		addr.ToString(true,ipport,'|');
		m_ConIPInfoMap.lock(false);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
			It->second->State=KKAutoDisConnect;
			//FreeKcpInfo(It,true);
		}
		m_ConIPInfoMap.unlock(false);
	}else if(msgId==KKSYN){/*******服务器接收*********/
		unsigned long  SYNvalue=ntohl(*(unsigned long*)(buf+4));
		unsigned short SYNMTU=ntohs(*(unsigned short*)(buf+8));
		
		/***协商MTU****/
		if(SYNMTU>buflen)
			SYNMTU=buflen-1;
		if(SYNvalue>m_CurConv){
			m_CurConv=SYNvalue;
		}else{
			m_CurConv++;
		}
		
		if(nTranId>0){
			addr.ToString(false,ipport,'|');
			addr.FromString(ipport,addr.GetPort(),nTranId);
		}
		
		addr.ToString(true,ipport,'|');
#ifdef ConPrintLg
		printf("\n KKSYN %s\n",ipport);
#endif
		KcpInfo* info = new KcpInfo();
		m_ConIPInfoMap.lock(false);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
		         FreeKcpInfo(It,false);
		}
		
		
		info->Send=&m_kksocket;
		info->Conv=m_CurConv;
		info->mtu=SYNMTU;
		info->State=KKSYNACK;
		info->Addr=addr;
		if(nTranId>0)
		{
			info->bTranspond=true;
			info->nTranId=nDestTranId;
			info->nDestTranId=nTranId;
		}else{
			info->bTranspond=false;
		}
		m_ConIPInfoMap.Key1Insert(ipport,info);

		m_ConIPInfoMap.unlock(false);
		SendSynAck(info->Conv,SYNMTU,addr,info->nTranId,info->nDestTranId);
	}else if(msgId== KKSYNACK){/*********客户端接收************/
		unsigned long conv=ntohl(*(unsigned long*)(buf+4));
		unsigned short mtu=ntohs(*(unsigned short*)(buf+8));
		KcpInfo*  pKcpInfo=NULL;
#ifdef ConPrintLg
	    printf("KKSYNACK \n");
#endif		
		if(nTranId>0){
			addr.ToString2(ipport,nTranId,'|');
		}else{
            addr.ToString(true,ipport,'|');
		}
		m_ConIPInfoMap.lock(false);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
			It->second->State=KKACK;
			It->second->sendtick=KKTickCount();
			It->second->kcp=CreateKcp(ipport,conv,mtu);
			It->second->Conv=conv;
			It->second->HandShake=0;
			It->second->mtu=mtu;
			pKcpInfo=It->second;
			
			m_ConIPInfoMap.key2insert( ipport,It->second->Conv,It->second);
		}else{
			assert(0);
		}
		m_ConIPInfoMap.unlock(false);
		if(pKcpInfo!=NULL)
		SendAck(conv,mtu,addr,pKcpInfo->nTranId,pKcpInfo->nDestTranId);
	}else if(msgId==KKACK){/******服务端接收*******/
		/********************/
		unsigned long conv=ntohl(*(unsigned long*)(buf+4));
		unsigned short mtu=ntohs(*(unsigned short*)(buf+8));
#ifdef ConPrintLg
		printf("KKACK mtu:%d\n",mtu);
#endif		
		if(nTranId>0){
			addr.ToString2(ipport,nTranId,'|');
		}else{
			addr.ToString(true,ipport,'|');
		}

		m_ConIPInfoMap.lock(false);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
			if(It->second->State==KKSYNACK&&It->second->Conv==conv&&It->second->mtu==mtu){
                 It->second->State=KKLink;
				 It->second->sendtick=KKTickCount();
				 It->second->kcp=CreateKcp(ipport,conv,mtu);
				 m_ConIPInfoMap.key2insert( ipport,It->second->Conv,It->second);

				 KK_Packet *packet= new KK_Packet();
				 packet->ConId=conv;
				 packet->msgId=MSGLinkIncoming;
				 packet->Addr=It->second->Addr;
				 PutKKPacket(packet);

			}else{

				 It->second->sendtick=KKTickCount();
#ifdef ConPrintLg
				 printf("KKACK %d,%d\n",It->second->Conv,conv);
#endif
			}
			
		}else{
			assert(0);
		}
		m_ConIPInfoMap.unlock(false);

		
		SendLink(conv,mtu,addr,nDestTranId,nTranId);
		
	}else if(msgId==KKLink){
        /********客户端接受**********/
#ifdef ConPrintLg
		printf("客户端接受 KKLink \n");
#endif
		/********************/
		unsigned long conv=ntohl(*(unsigned long*)(buf+4));
		unsigned short mtu=ntohs(*(unsigned short*)(buf+8));
		if(nTranId>0)
		    addr.ToString2(ipport,nTranId,'|');
		else
			addr.ToString(true,ipport,'|');
		m_ConIPInfoMap.lock(true);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
			if(It->second->Conv==conv&&It->second->mtu==mtu){
				It->second->State=KKLink;
				It->second->sendtick=KKTickCount();
				It->second->HandShake=0;
				KK_Packet *packet= new KK_Packet();
				memset(packet,0,packetlen);
				packet->msgId=MSGLinkAccepted;
				packet->ConId=conv;
				packet->Addr=It->second->Addr;
				PutKKPacket(packet);
			}else{
				SendKckLost(conv,addr);
			}
		}else{
			SendKckLost(conv,addr);
		}
		m_ConIPInfoMap.unlock(true);
	}else if(msgId==KKKcpHeart){/********kcp 心跳********/
		
		
		unsigned long conv=ntohl(*(unsigned long*)(buf+4));
		unsigned char HeartTick=*(buf+5);
		m_ConIPInfoMap.lock(true);
		std::map<IUINT32,KcpInfo*>::iterator It=m_ConIPInfoMap.Key2Find(conv);
		if(It!=m_ConIPInfoMap.Key2End()){
			
			It->second->sendtick=KKTickCount();
			
			if(!It->second->sponsor){
				//服务端，收到客户端心跳，回复心跳
#ifdef ConPrintLg
				printf("客户端 KKKcpHeart \n");
#endif
				SendKcpHeart(It->second->Conv,0,It->second->Addr,It->second->nTranId,It->second->nDestTranId);
			}else{
				///收到服务器端的心跳
				It->second->rectick=It->second->sendtick;
#ifdef ConPrintLg
				printf("服务端 KKKcpHeart \n");
#endif
			}
			KK_Packet *packet= new KK_Packet();
			memset(packet,0,packetlen);
			packet->msgId=MSGKcpHeart;
			packet->ConId=conv;
			packet->Addr=It->second->Addr;
			PutKKPacket(packet);
			
		}else{
			SendKckLost(conv,addr);
		}
		m_ConIPInfoMap.unlock(true);
	}else if(msgId==KKUnLink){
		KK_Packet *packet= new KK_Packet();
		memset(packet,0,packetlen);
		packet->msgId=MSGUnLink;
		packet->Addr=addr;
		PutKKPacket(packet);
	}else if(msgId==KKKckLost){
		KK_Packet *packet= new KK_Packet();
		memset(packet,0,packetlen);
		packet->msgId=MSGKckLost;
		packet->Addr=addr;
		
		m_ConIPInfoMap.lock(false);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
			It->second->State=KKKckLost;
			packet->ConId=It->second->Conv;
			FreeKcpInfo(It,true);
		}
		m_ConIPInfoMap.unlock(false);

		PutKKPacket(packet);
	}
}
void CKKkcp::SendLink(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	unsigned short OutLen=0;
	char* bufref=(char*)putlink(cov,mtu,OutLen);
	SendSign(bufref,OutLen,nTranId,nDestTranId,addr);
	KKfree(bufref);
}
void CKKkcp::OnTranBufHandle(unsigned char* buf,unsigned short buflen,KKAddress &addr)
{
	unsigned char msgId=*buf;
	static int packetlen=sizeof(KK_Packet);
	char ipport[256]="";
	//得到服务器转发Id
	if(msgId==UDPGETTRANID){
		addr.ToString(true,ipport,'|');
		m_ConIPInfoMap.lock(false);
		std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
		if(It!=m_ConIPInfoMap.Key1End()){
			It->second->State=KKTranIdOk;
			It->second->sendtick=KKTickCount();
			It->second->nTranId=ntohl(*(unsigned long*)(buf+1));
			KK_Packet *packet=(KK_Packet *)KKmalloc(packetlen);
			memset(packet,0,packetlen);
			packet->ConId=It->second->nTranId;
			packet->msgId=MSGTranIdOk;
			packet->Addr=It->second->Addr;
			PutKKPacket(packet);
		}
		m_ConIPInfoMap.unlock(false);
	}
}
void CKKkcp::OnKcp(unsigned char* buf,unsigned short buflen,IUINT32 nTranId,IUINT32 nDestTranId,KKAddress &addr)
{
	char ipport[256]="";
	
	if(nTranId>0){
		 addr.ToString2(ipport,nTranId,'|');
	}else{
         addr.ToString(true,ipport,'|');
	}
	
	KcpInfo* Info=NULL;
	m_ConIPInfoMap.lock(true);
	std::map<std::string,KcpInfo*>::iterator It=m_ConIPInfoMap.Key1Find(ipport);
	if(It!=m_ConIPInfoMap.Key1End()){
		Info=It->second;
		Info->AddRef();
	}else{
		SendUnLink(addr,nTranId,nDestTranId);
	}
	m_ConIPInfoMap.unlock(true);

	if(Info!=NULL){
	    ikcpcb *kcp=Info->kcp;
		Info->rectick=Info->sendtick=::KKTickCount();
		if(Info->kcp!=NULL){ 
			unsigned int ic=It->second->sendtick& 0xfffffffful;
			m_KcpLock.Lock();     
			ikcp_input(Info->kcp,(char*)buf, buflen);
			ikcp_update(Info->kcp,ic);
			GetRecKcpData(Info->kcp,addr);
			m_KcpLock.UnLock();
		}
		Info->Release();
	}
}
void CKKkcp::GetRecKcpData(ikcpcb *kcp,KKAddress& Addr)
{
	static int packtlen=sizeof(KK_Packet);
	
	int DataLen=ikcp_recv(kcp,m_pRecKcpbuf,MAXBUF);
	if(DataLen>0){	
		KK_Packet* packet= new KK_Packet();
		packet->Addr=Addr;
		packet->msgId=MSGKcpData;
		packet->ConId=kcp->conv;
		packet->buf=(unsigned char* )::KKmalloc(DataLen);
		memmove(packet->buf,m_pRecKcpbuf,DataLen);
		packet->len=DataLen;
		PutKKPacket(packet);
	}
	
}
ikcpcb*  CKKkcp::CreateKcp(std::string Ipport,IUINT32 conv,unsigned short mtu)
{
	ikcpcb *kcp = NULL;
	std::map<std::string,KcpInfo*>::iterator It;
	KcpInfo* info =NULL;
	It= m_ConIPInfoMap.Key1Find(Ipport);
	if(It!=m_ConIPInfoMap.Key1End()){
		info =It->second;
		if(info!=NULL){
			/*if(mtu>1400)
				;
			mtu=500;*/
			kcp =ikcp_create(conv, info);
			ikcp_nodelay(kcp, 1, 5, 2, 1);
			//ikcp_nodelay(kcp, 1, 5, 1, 1);
			ikcp_setmtu(kcp,mtu-1);
			//ikcp_wndsize(kcp, 128, 128);
			kcp->rx_minrto = 10;
			info->kcp=kcp;
			//info->State=KKACK;
			info->Conv=conv;
		}
	}else{
		assert(0);
	}
	kcp->output= kk_udp_output;/**/

	return kcp;
}
void CKKkcp::GenSysKKPacket(KKConState sys,KKAddress& Addr,unsigned long conId)
{
	int len=sizeof(KK_Packet);
	KK_Packet *packet = new KK_Packet();
	packet->Addr=Addr;
	packet->ConId=conId;
	if(sys==KKLink||sys==KKKckLost){
	   packet->msgId=MSGLinkLost;
	}else if(sys<=KKACK){
		packet->msgId=MSGConnectFail;
	}else if(sys==KKAutoDisConnect){
		packet->msgId=MSGKcpDisConnect;
	}else if(sys==KKGetTranId){
	    packet->msgId=MSGTranConnectFail;
	}
	PutKKPacket(packet);
}
KK_Packet* CKKkcp::Receive()
{
	KK_Packet* p=NULL;
	m_RecPacketQueLock.Lock();
	if(m_RecPacketQue.size()>0)
	{
		 p=m_RecPacketQue.front();
		 m_RecPacketQue.pop();
	}
	m_RecPacketQueLock.UnLock();
	return p;
}
void CKKkcp::DelPacket(KK_Packet* packet)
{
	KKfree(packet->buf);
	delete packet;
}
void CKKkcp::PutKKPacket(KK_Packet* packet)
{
	m_RecPacketQueLock.Lock();
	m_RecPacketQue.push(packet);
	m_RecPacketQueLock.UnLock();
}
void CKKkcp::FreeKcpInfo(std::map<std::string,KcpInfo* >::iterator &KcpIt,bool NeedMsg)
{
	char ipport[256]="";
	KcpInfo* pKcpInfo=KcpIt->second;
	IUINT32  conId=KcpIt->second->Conv;
	pKcpInfo->Addr.ToString(true,ipport,'|');
	
	
	KcpIt=m_ConIPInfoMap.Key1Del(ipport);
	if(NeedMsg)
	   GenSysKKPacket(pKcpInfo->State,pKcpInfo->Addr,conId);
	pKcpInfo->Release();
}

void CKKkcp::SendKcpDisconnect(IUINT32 cov,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	
	unsigned short OutLen=0;
	char* bufref=NULL;
	if(nTranId>0&&nDestTranId==0){
		bufref=(char*)putDelTranId(OutLen);
	}else{
		bufref=(char*)putkcpdisconnect(cov,OutLen);
	}
	
	SendSign(bufref,OutLen,nTranId,nDestTranId,addr);
	KKfree(bufref);
}
void CKKkcp::SendKckLost(IUINT32 cov,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	unsigned short OutLen=0;
	char* bufref=(char*)putkcplost(cov,OutLen);
	SendSign(bufref,OutLen,nTranId,nDestTranId,addr);
	KKfree(bufref);
}
void CKKkcp::SendSyn(IUINT32 cov,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	unsigned short OutLen=0;
	char* bufref=(char*)putsyn(cov,OutLen,nDestTranId);
	SendSign(bufref,OutLen,nTranId,nDestTranId,addr);
	KKfree(bufref);
}
void CKKkcp::SendSynAck(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	unsigned short OutLen=0;
	char* bufref=(char*)putsynack(cov,mtu,OutLen);
	SendSign(bufref,OutLen,nTranId,nDestTranId,addr);
	KKfree(bufref);
}
void CKKkcp::SendAck(IUINT32 cov,unsigned short mtu,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	unsigned short OutLen=0;
	char* bufref=(char*)putack(cov,mtu,OutLen);
	SendSign(bufref,OutLen,nTranId,nDestTranId,addr);
	KKfree(bufref);
	
}
void CKKkcp::SendUnLink(KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
   unsigned short OutLen=0;
   unsigned char* buf=putunlink(OutLen);
   SendSign((char*)buf,OutLen,nTranId,nDestTranId,addr);
   KKfree(buf);
}
void CKKkcp::SendKcpHeart(IUINT32 Conv,unsigned char HeartTick,KKAddress& addr,IUINT32 nTranId,IUINT32 nDestTranId)
{
	unsigned short OutLen=0;
	unsigned char* heartbuf= putkcpheart(Conv,OutLen);
	SendSign((char*)heartbuf,OutLen,nTranId,nDestTranId,addr);
	KKfree(heartbuf);
}
/**********得到自身的转发Id************/
void CKKkcp::SendGetTranId(KKAddress& addr)
{
	unsigned short OutLen=0;
	char* bufref=(char*)putGetTranId(OutLen);
	this->m_kksocket.kksend(bufref,OutLen,addr);
	KKfree(bufref);
}
//发送数据
void CKKkcp::SendSign(char* bufref,unsigned short &OutLen,IUINT32 &nTranId,IUINT32 &nDestTranId,KKAddress& addr)
{
	char* pTemp=(char*)KKmalloc(2048);
	if(nDestTranId>0){
		butTranspondHeadr((unsigned char*)pTemp,OutLen,nTranId,nDestTranId);
		memcpy(pTemp+9,bufref,OutLen);
		OutLen+=9;
	}else{
		memcpy(pTemp,bufref,OutLen);
	}
	m_kksocket.kksend(pTemp,OutLen,addr);
	KKfree(pTemp);
}