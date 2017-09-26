#ifndef RtcConmgr_H_
#define RtcConmgr_H_
#include "stdafx.h"
#include <map>
#include <string>
class CRtcConmgr
{
	public:
	        CRtcConmgr();
			~CRtcConmgr();
			int AddRtcConn(std::string peerId,std::shared_ptr<erizo::WebRtcConnection> conn);
	private:
	     std::map<std::string,std::shared_ptr<erizo::WebRtcConnection>>  m_peerIdRtcConnMap;
	
};
#endif