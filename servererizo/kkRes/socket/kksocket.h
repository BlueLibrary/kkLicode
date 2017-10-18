#ifndef kk_socket_H_
#define kk_socket_H_
#include "kkAddr.h"
#include "../Base/KKMutex.h"
namespace kkBase
{
	class ckksocket
	{
	    public:
			      ckksocket();
			      ~ckksocket();
			      int getsocket();
				  ///绑定端口号及Ip。
			      int kkbind(const char *Ip,unsigned short port);
				  //发送报文数据
			      int kksend(const char *buf,unsigned short len,KKAddress &adr);
				  //接收报文数据
			      int kkrecvfrom(char *buf,unsigned short len,KKAddress &adr);
		public:
				  unsigned char *m_pTemp;
		private:
				  int            m_socket;
				 // FD_SET         m_readSet;
				  //
				  CKKMutex       m_mutex;
	};
}
#endif