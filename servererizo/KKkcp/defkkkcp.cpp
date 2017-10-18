#include "defkkkcp.h"
#include <stdio.h>
KcpInfo::KcpInfo()
{
	Conv=0;
	kcp=NULL;
	Send=NULL;
	mtu=0;
	State=KKUnLink;
	sponsor=false;
	HandShake=0;
	bTranspond=false;
	nTranId=0;
	nDestTranId=0;
	rectick=sendtick=KKTickCount();
	
	PackId=0;
}
KcpInfo::~KcpInfo()
{
	ikcp_release(kcp);
#ifdef _DEBUG
	printf("~KcpInfo Id=%d \n",Conv);
#endif
}
void* KcpInfo::operator new(size_t size)
{
	return KKmalloc(size);
}
void KcpInfo::operator delete( void *ptr)
{
	KKfree(ptr);
}


CKcpGurd::CKcpGurd(KcpInfo& KcpObj):KcpInfoObj(KcpObj)
{
	KcpInfoObj.AddRef();
}
CKcpGurd::~CKcpGurd()
{
	KcpInfoObj.Release();
}



KK_Cmd_Info::KK_Cmd_Info()
{
	buf=0;
	len=0;
	Conv=0;   
	nTranId=0;
	nDestTranId=0;
	cmd=CMDUnKown;
}

void*  KK_Cmd_Info::operator new(size_t size)
{
   return KKmalloc(size);
}
void  KK_Cmd_Info::operator  delete( void *ptr)
{
	KKfree(ptr);
}

KK_Packet::KK_Packet()
{
	buf=0;
	len=0;
	ConId=0;
	msgId=MSGUnKown;
}
void* KK_Packet::operator new(size_t size)
{
	return KKmalloc(size);
}
void KK_Packet::operator delete( void *ptr)
{
	KKfree(ptr);
}
unsigned char* putsyn(unsigned long cov,unsigned short &OutLen,unsigned int nDestTranId)
{
	unsigned short MTUMAX=1500-64;//
	if(nDestTranId>0){
		MTUMAX-=9;
	}
	OutLen= MTUMAX;
	unsigned char *DataSYN=(unsigned char *)KKmalloc(MTUMAX);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,MTUMAX);
	*DataSYN=0xAF;
	temp=temp+1;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKSYN);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	unsigned short* SYNMTU=(unsigned short *)(temp);
	*SYNMTU=htons(MTUMAX);

	return DataSYN;
}
unsigned char* putsynack(unsigned long cov,unsigned short mtu,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKSYNACK);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	unsigned short* SYNMtu=(unsigned short *)(temp);
	*SYNMtu=htons(mtu);
	temp+=2;

	OutLen=15;
	return DataSYN;
}
unsigned char* putack(unsigned long cov,unsigned short mtu,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKACK);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	unsigned short* SYNMtu=(unsigned short *)(temp);
	*SYNMtu=htons(mtu);
	temp+=2;

	OutLen=15;
	return DataSYN;
}
unsigned char* putlink(unsigned long cov,unsigned short mtu,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKLink);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	unsigned short* SYNMtu=(unsigned short *)(temp);
	*SYNMtu=htons(mtu);
	temp+=2;

	OutLen=15;
	return DataSYN;
}
unsigned char* putkcpheart(unsigned long cov,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKKcpHeart);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	

	OutLen=15;
	return DataSYN;
}
unsigned char* putkcpdisconnect(unsigned long cov,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKDisConnect);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	OutLen=15;
	return DataSYN;
}

unsigned char* putunlink(unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKUnLink);
	temp+=4;

	

	OutLen=15;
	return DataSYN;
}
unsigned char* butTranId(unsigned long cov,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAC;
	temp++;
	


	OutLen=15;
	return DataSYN;
}
unsigned char* putkcplost(unsigned long cov,unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAF;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=htonl(UDPCTLMSG);
	temp+=4;

	unsigned int* SYN=(unsigned int *)(temp);
	*SYN=htonl(KKKckLost);
	temp+=4;

	unsigned int* SYNvalue=(unsigned int *)(temp);
	*SYNvalue=htonl(cov);
	temp+=4;

	OutLen=15;
	return DataSYN;
}
unsigned char* putGetTranId(unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAC;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=UDPGETTRANID;
	temp+=1;

	
	OutLen=15;
	return DataSYN;
}
unsigned char* putDelTranId(unsigned short &OutLen)
{
	unsigned char *DataSYN=(unsigned char *)KKmalloc(15);
	unsigned char *temp=DataSYN;
	memset(DataSYN,0,15);
	*DataSYN=0xAC;
	temp++;
	unsigned int* msgId=(unsigned int *)(temp);
	*msgId=UDPDELTRANID;
	temp+=1;


	OutLen=15;
	return DataSYN;
}
int butTranspondHeadr(unsigned char* Buf,unsigned short bufLen,IUINT32 nTranId,IUINT32 nDestTranId)
{
	if(bufLen<5)
		return 0;
	*Buf=0xAD;
	Buf+=1;
	unsigned int* TranId=(unsigned int *)Buf;
	*TranId=htonl(nTranId);
	Buf+=4;

	unsigned int* DestTranId=(unsigned int *)Buf;
	*DestTranId=htonl(nDestTranId);
	Buf+=4;

	return 9;
}
unsigned int kk_check_sum(unsigned char * data,unsigned int  DataSize)
{
	if ((data == NULL) || (DataSize==0))
	{
		return 0;
	}

	register unsigned int   sum  = 0;
	register unsigned int   iter = DataSize;
	register unsigned char  *bptr = data;

	while (iter-- > 0){
		sum += *bptr;
		bptr++;
	}
	return sum;
}