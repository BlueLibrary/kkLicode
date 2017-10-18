#include "kksocket.h"
#ifdef WIN32
    #include <Winsock2.h>  
#else
	#include <fcntl.h> 
    #define SOCKET_ERROR -1
#endif
#include <assert.h>


namespace kkBase
{

	
	static int makeSocketNonBlocking(int sock)
	{
		#if defined(__WIN32__) || defined(_WIN32)
			unsigned long arg = 1;
			return ioctlsocket(sock, FIONBIO, &arg) == 0;
		#elif defined(VXWORKS)
			int arg = 1;
			return ioctl(sock, FIONBIO, (int)&arg) == 0;
		#else
			int curFlags = fcntl(sock, F_GETFL, 0);
			return fcntl(sock, F_SETFL, curFlags|O_NONBLOCK) >= 0;
		#endif
	}
	ckksocket::ckksocket()
	{
		static bool Ini=false;
		if(!Ini)
		{
			Ini=true;

			#ifdef _WINDOWS
			WSADATA wsaData;  
			struct addrinfo *result = NULL, *ptr = NULL, hints;  
			if(WSAStartup(MAKEWORD(2, 2), &wsaData))  
			{  
			
			}  	
			#endif
		}
		unsigned long ul=1;

	
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	#ifdef _WINDOWS
		if(m_socket  == INVALID_SOCKET){
			assert(0);
		}
	#else
	    if(m_socket  < 0){
			assert(0);
		}
    #endif
		
		///设置成非堵塞
		int ret=makeSocketNonBlocking(m_socket);
		if(ret<1){
			assert(0);
		}/**/
		m_pTemp=(unsigned char *)KKmalloc(2048);

		
	}
	ckksocket::~ckksocket()
	{
		KKfree(m_pTemp);
		m_pTemp=NULL;
	}
	int ckksocket::getsocket(){
		return m_socket;
	}
	int ckksocket::kkbind(const char *Ip,unsigned short port)
	{
			sockaddr_in serAddr;
			serAddr.sin_family = AF_INET;
			serAddr.sin_port = htons(port);
			serAddr.sin_addr.s_addr =  inet_addr(Ip); 
			char* llxx=inet_ntoa(serAddr.sin_addr);
			if(bind(m_socket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
				assert(0);
				return 0;
			}
			return 1;
	}
	int ckksocket::kksend(const char *buf,unsigned short len,KKAddress &adr)
	{
		static int llxx=sizeof(sockaddr_in);
		m_mutex.Lock();
		int ret=sendto(m_socket, buf,len, 0, (sockaddr *)&adr.address.addr4, llxx);
		m_mutex.UnLock();
		/*if(ret<=0){
			KKSleep(10);
			ret=sendto(m_socket, buf,len, 0, (sockaddr *)&adr.address.addr4, llxx);
			if(ret<=0){
				assert(0);
			}
		}*/
		return ret;
	}
	int ckksocket::kkrecvfrom(char *buf,unsigned short len,KKAddress &adr)
	{
		
		
		static socklen_t lx=sizeof(sockaddr_in);
		return	recvfrom(m_socket, buf,len,0 , (sockaddr *)&adr.address.addr4, &lx);
		//static timeval timeout;    // 定时变量
		//timeout.tv_sec = 0;   
  //      timeout.tv_usec = 5000; 

		//FD_ZERO(&m_readSet);
  //      FD_SET((SOCKET)m_socket,&m_readSet);

		//int nRet = select(0, &m_readSet, NULL, NULL, &timeout);

		//if (nRet == SOCKET_ERROR)     
  //      {  
  //         
  //      }  
  //      else if (nRet == 0) // 超时  
  //      {  
  //         
  //      }  
  //      else    // 检测到有套接口可读  
  //      {  
  //          if (FD_ISSET(m_socket, &m_readSet))  // socka可读  
  //          {  
		//		return	recvfrom(m_socket, buf,len,0 , (sockaddr *)&adr.address.addr4, &lx);
		//       
		//	}
		//}
		//return -1;
	}
}
