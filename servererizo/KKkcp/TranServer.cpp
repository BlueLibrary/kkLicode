#include "TranServer.h"
#define MaxTranTick     120000
TranInfo::TranInfo()
{
	Id=0;
}
CKKTranServer::CKKTranServer():m_PreOk(0),m_Id(1)
{

}
CKKTranServer::~CKKTranServer()
{

}
int CKKTranServer::KkBind(char *Ip,unsigned short port)
{
	return m_kksocket.kkbind(Ip,port);
}
void CKKTranServer::Start()
{
	if(m_PreOk==0)
	{
		m_PreOk=1;
#ifdef WIN32
		m_RecThread_info.ThreadHandle=(HANDLE)_beginthreadex(NULL, NULL,RecUdpThread, (LPVOID)this, 0,&m_RecThread_info.ThreadAddr);
#else
		m_RecThread_info.ThreadAddr =  pthread_create(&m_RecThread_info.Tid_task, NULL, (void* (*)(void*))RecUdpThread, (void*)this);
#endif
	}
}
unsigned __stdcall CKKTranServer::RecUdpThread(void *pUserData)
{
	CKKTranServer *pIns=( CKKTranServer *)pUserData;
	pIns->HandleRecUdp();
	return 0;
}

void CKKTranServer::FreeTranInfo(std::map<std::string,TranInfo* >::iterator &KcpIt,bool NeedMsg)
{
	char ipport[256]="";
	TranInfo* pKcpInfo=KcpIt->second;
	pKcpInfo->Addr.ToString(true,ipport,'|');
	KcpIt=m_TranInfoMap.Key1Del(ipport);
	KKfree(pKcpInfo);
}
void  CKKTranServer::HandleRecUdp()
{
	unsigned short bufLen=2048, OutLen=0;
	KKAddress adr;
	int RecLen=0;
	char *RecBuf=(char*)KKmalloc(bufLen);
	unsigned long long millisec=0,kcptick=0;
	std::map<std::string,TranInfo*>::iterator It;
	TranInfo* TInfo=0;
	while(m_PreOk==1)
	{
		RecLen=m_kksocket.kkrecvfrom(RecBuf,bufLen,adr);
		if(RecLen>0){
			OnTranBufHandle((unsigned char*)RecBuf,bufLen,adr);
		}else{
			millisec=KKTickCount();
			m_TranInfoMap.lock(false);
			It=m_TranInfoMap.Key1begin();
			for(;It!=m_TranInfoMap.Key1End();){
				  TInfo=It->second;
			      kcptick=millisec-TInfo->Tick;
				  /******释放连接信息*****/
				  if(kcptick>MaxTranTick)
				  {
					     printf("\nId:%d 心跳丢失！",It->second->Id);
					     FreeTranInfo(It,false);
				  }else{
					  ++It;
				  }
			}
			m_TranInfoMap.unlock(false);
			KKSleep(10);
		}
	}
}
void  CKKTranServer::SendTranId(unsigned int Id,KKAddress &add)
{
	unsigned char buf[6]={0xAC,(unsigned char)UDPGETTRANID};
	Id=htonl(Id);
	memcpy(buf+2,&Id,4);
	m_kksocket.kksend((char*)buf,6,add);
}
void CKKTranServer::SendNotifyCode(int errcode,unsigned int Id,KKAddress &add)
{
	unsigned char buf[256]={0xAC,0};
	if(errcode==UDPUNDEFTRANID)
	{
		buf[1]=errcode;
		unsigned int *TId=(unsigned int *)&buf[2];
		*TId=htonl(Id);
	}else if(errcode==UDPUNTRANID)
	{
		buf[1]=errcode;
	}
	this->m_kksocket.kksend((char*)buf,256,add);
}
void CKKTranServer::OnTranBufHandle(unsigned char* buf,unsigned short buflen,KKAddress &addr)
{
	unsigned char  typex=*buf;
	unsigned long  conv=0;
	unsigned long  conv2=0;
	
	char ipport[256]="";
	addr.ToString(true,ipport,'|');

	/***********转发控制头**************/
	if(typex==0xAC){
		buf++;
		unsigned char msgId=*buf;
		if(msgId==UDPDELTRANID)
		{
			std::map<std::string,TranInfo*>::iterator It;
			m_TranInfoMap.lock(false);
			It=m_TranInfoMap.Key1Find(ipport);
			if(It!=m_TranInfoMap.Key1End())
			{
				printf("\nId:%d 断开！",It->second->Id);
				FreeTranInfo(It,false);
			}

			m_TranInfoMap.unlock(false);
		}/***获取转发Id***/
		else if(msgId==UDPGETTRANID){
			std::map<std::string,TranInfo*>::iterator It;
			m_TranInfoMap.lock(false);
			It=m_TranInfoMap.Key1Find(ipport);
			if(It!=m_TranInfoMap.Key1End()){
				  It->second->Tick=::KKTickCount();
				  printf("\n获取Id:%d %s",It->second->Id,ipport);
			      SendTranId(It->second->Id,addr);
			}else{
				TranInfo *info = new TranInfo();
				std::set<unsigned int>::iterator ItSet;
				while(1){
					ItSet=m_IdSet.find(m_Id);
					if(ItSet!=m_IdSet.end()){
						++m_Id;
						if(m_Id>0xfffe){
							m_Id=0;
						}
					}else{
						break;
					}
				}
				
				info->Id=m_Id;
				printf("\nId:%d %s",info->Id,ipport);
				m_IdSet.insert(info->Id);
				info->Addr=addr;
				info->Tick=::KKTickCount();
				m_TranInfoMap.insert(ipport,info->Id,info);
				SendTranId(info->Id,addr);
			}
			m_TranInfoMap.unlock(false);
		}else if(msgId==UDPLOOKPRETRAN){
			unsigned int* TranId=(unsigned int *)(buf+1);
			unsigned int  nTranId=ntohl(*TranId);
			std::map<unsigned int,TranInfo*>::iterator It2;
			std::map<std::string,TranInfo*>::iterator It;
			unsigned char temp[64]={0xAC,UDPLOOKPRETRAN,0,*(buf+1),*(buf+2),*(buf+3),*(buf+4)};
			
			m_TranInfoMap.lock(false);
            It=m_TranInfoMap.Key1Find(ipport);

			if(It!=m_TranInfoMap.Key1End()){
				It->second->Tick=::KKTickCount();
				SendTranId(It->second->Id,addr);
			}

			It2=m_TranInfoMap.Key2Find(nTranId);
			if(It2!=m_TranInfoMap.Key2End()){
				temp[2]=1;
			}else{
				temp[2]=0;
			}
			m_kksocket.kksend((const char*)temp,7,addr);
			m_TranInfoMap.unlock(false);
		}else if(msgId==UDPTranHeart){
			std::map<std::string,TranInfo*>::iterator It;
			m_TranInfoMap.lock(false);
			It=m_TranInfoMap.Key1Find(ipport);

			if(It!=m_TranInfoMap.Key1End()){
				It->second->Tick=::KKTickCount();
				printf("\nId:%d %s 心跳",It->second->Id,ipport);
			}
			m_TranInfoMap.unlock(false);
			
		}
	}else if(typex==0xAD)//消息转发
	{
		unsigned int* TranId=(unsigned int *)(buf+1);
		unsigned int  nTranId=ntohl(*TranId);
		
		unsigned int* DestTranId=(unsigned int *)(buf+5);
		unsigned int  nDestTranId=ntohl(*DestTranId);
		std::map<unsigned int,TranInfo*>::iterator It2;

		std::map<std::string,TranInfo*>::iterator It;
		m_TranInfoMap.lock(false);
		It=m_TranInfoMap.Key1Find(ipport);
		if(It!=m_TranInfoMap.Key1End()){
			It->second->Tick=KKTickCount();
			It2=m_TranInfoMap.Key2Find(nDestTranId);
			if(It2!=m_TranInfoMap.Key2End()){
				this->m_kksocket.kksend((char*)buf,buflen,It2->second->Addr);
			}else{
				/*******远程转发Id未定义********/
				SendNotifyCode(UDPUNDEFTRANID,nDestTranId,addr);
			}
		}else{
			SendNotifyCode(UDPUNTRANID,0,addr);
		}
		m_TranInfoMap.unlock(false);
	}
}