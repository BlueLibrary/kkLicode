#include "kkAddr.h"
char* Itoa( int value, char* result, int base )
 {
	// check that the base if valid
	if (base < 2 || base > 16) { *result = 0; return result; }
	char* out = result;
	int quotient = value;

	int absQModB;

	do {
		// KevinJ - get rid of this dependency
		//*out = "0123456789abcdef"[ std::abs( quotient % base ) ];
		absQModB=quotient % base;
		if (absQModB < 0)
			absQModB=-absQModB;
		*out = "0123456789abcdef"[ absQModB ];
		++out;
		quotient /= base;
	} while ( quotient );

	// Only apply negative sign for base 10
	if ( value < 0 && base == 10) *out++ = '-';

	// KevinJ - get rid of this dependency
	// std::reverse( result, out );
	*out = 0;

	// KevinJ - My own reverse code
    char *start = result;
	char temp;
	out--;
	while (start < out)
	{
		temp=*start;
		*start=*out;
		*out=temp;
		start++;
		out--;
	}

	return result;
}
bool KKAddress::EqualsExcludingPort( const KKAddress& right ) const
{
	return (address.addr4.sin_family==AF_INET && address.addr4.sin_addr.s_addr==right.address.addr4.sin_addr.s_addr)
#if KKNET_SUPPORT_IPV6==1
		|| (address.addr4.sin_family==AF_INET6 && memcmp(address.addr6.sin6_addr.s6_addr, right.address.addr6.sin6_addr.s6_addr, sizeof(address.addr6.sin6_addr.s6_addr))==0)
#endif
		;
}

KKAddress::KKAddress()
{
	DestTranId=0;
}
KKAddress::KKAddress(const char *str, unsigned short port,kk_ulong nDestTranId)
{
	address.addr4.sin_family=AF_INET;
	address.addr4.sin_addr.s_addr =inet_addr(str);
	address.addr4.sin_port=htons(port);
	DestTranId=nDestTranId;
}
bool KKAddress::FromString(const char *str, unsigned short port,kk_ulong nDestTranId, int ipVersion)
{
	address.addr4.sin_family=AF_INET;
	address.addr4.sin_addr.s_addr=inet_addr(str);
	address.addr4.sin_port=htons(port);
	DestTranId=nDestTranId;
	return true;
}
unsigned short KKAddress::GetPort() const
{
	return ntohs(address.addr4.sin_port);
}
void KKAddress::ToString(bool writePort, char *dest, char portDelineator) const
{
	char portStr[2];
	portStr[0]=portDelineator;
	portStr[1]=0;

	in_addr in;
	in.s_addr = address.addr4.sin_addr.s_addr;
	const char *ntoaStr = inet_ntoa( in );
	strcpy(dest, ntoaStr);
	if (writePort)
	{
		strcat(dest, portStr);
		Itoa(ntohs(address.addr4.sin_port), dest+strlen(dest), 10);
		if(DestTranId>0){
			strcat(dest, portStr);
			Itoa(DestTranId, dest+strlen(dest), 10);
		}
	}
}
void KKAddress::ToString2(char *dest,kk_ulong nDestTranId, char Delineator) const
{
	char portStr[2];
	portStr[0]=Delineator;
	portStr[1]=0;

	in_addr in;
	in.s_addr = address.addr4.sin_addr.s_addr;
	const char *ntoaStr = inet_ntoa( in );
	strcpy(dest, ntoaStr);
	strcat(dest, portStr);
	Itoa(ntohs(address.addr4.sin_port), dest+strlen(dest), 10);
	if(nDestTranId>0){
		strcat(dest, portStr);
		Itoa(nDestTranId, dest+strlen(dest), 10);
	}
}
bool KKAddress::operator==( const KKAddress& right ) const
{
	return address.addr4.sin_port == right.address.addr4.sin_port && EqualsExcludingPort(right);
}

bool KKAddress::operator<( const KKAddress& right ) const
{
	if (address.addr4.sin_port == right.address.addr4.sin_port)
	{
#if KKNET_SUPPORT_IPV6==1
		if (address.addr4.sin_family==AF_INET)
			return address.addr4.sin_addr.s_addr<right.address.addr4.sin_addr.s_addr;
		return memcmp(address.addr6.sin6_addr.s6_addr, right.address.addr6.sin6_addr.s6_addr, sizeof(address.addr6.sin6_addr.s6_addr))>0;
#else
		return address.addr4.sin_addr.s_addr<right.address.addr4.sin_addr.s_addr;
#endif
	}
	return address.addr4.sin_port<right.address.addr4.sin_port;
}


int GetIpV4ByHostName(const char *host_name,std::string &OutIp)
{
#ifdef WIN32
  HOSTENT *host_entry; 
  host_entry=gethostbyname(host_name);    
  if(host_entry!=0)  
  {  
	    char buf[64]="";
        sprintf(buf,"%d.%d.%d.%d",  
        (host_entry->h_addr_list[0][0]&0x00ff),  
        (host_entry->h_addr_list[0][1]&0x00ff),  
        (host_entry->h_addr_list[0][2]&0x00ff),  
        (host_entry->h_addr_list[0][3]&0x00ff)); 
		 OutIp=buf;
		 return 1;
  }  
#else
  struct hostent *host_entry;
  char buf[64]="";
        sprintf(buf,"%d.%d.%d.%d",  
        (host_entry->h_addr_list[0][0]&0x00ff),  
        (host_entry->h_addr_list[0][1]&0x00ff),  
        (host_entry->h_addr_list[0][2]&0x00ff),  
        (host_entry->h_addr_list[0][3]&0x00ff)); 
		 OutIp=buf;
		 return 1;
  //struct in_addr **addr_list;
  //if(host_entry!=0)  {     
	 //    addr_list = (struct in_addr **)host_entry->h_addr_list;
		// //OutIp=inet_ntoa(*addr_list[0]);
		//return 1;
  //}  
#endif
  
  return 0;
}