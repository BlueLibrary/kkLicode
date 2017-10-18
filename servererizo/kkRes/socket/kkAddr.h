#ifndef  kkAddr_H_
#define  kkAddr_H_

#ifdef WIN32
    #include <Winsock2.h> 
    #include <Windows.h>
    #include <process.h>
	#include "../Base/KKBase.h"
#else
	 #include "../Base/KKBase.h"
     #include <netdb.h>
	 #include <sys/types.h>
     #include <sys/socket.h>
	 #include <sys/un.h>
	 #include <netinet/in.h>
	 #include <arpa/inet.h>


#endif
#include <string>

typedef struct KKAddress
{
	union
	{
        #if KKNET_SUPPORT_IPV6==1
		   struct sockaddr_storage sa_stor;
		   sockaddr_in6 addr6;
        #endif
		sockaddr_in addr4;
	} address;
	kk_ulong DestTranId;
	KKAddress();
	KKAddress(const char *str, unsigned short port,kk_ulong nDestTranId);
    bool operator==( const KKAddress& right ) const;
    bool operator<( const KKAddress& right ) const;
    bool EqualsExcludingPort( const KKAddress& right ) const;
	bool FromString(const char *str, unsigned short port,kk_ulong nDestTranId=0,int ipVersion=0);
	void ToString(bool writePort, char *dest, char Delineator) const;
	void ToString2(char *dest,kk_ulong nDestTranId, char Delineator) const;
	unsigned short GetPort() const;
} KKAddress;
int GetIpV4ByHostName(const char *host_name,std::string &OutIp);
#endif