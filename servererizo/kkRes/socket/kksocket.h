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
				  ///�󶨶˿ںż�Ip��
			      int kkbind(const char *Ip,unsigned short port);
				  //���ͱ�������
			      int kksend(const char *buf,unsigned short len,KKAddress &adr);
				  //���ձ�������
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