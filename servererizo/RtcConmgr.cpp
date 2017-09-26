#include "RtcConmgr.h"
CRtcConmgr::CRtcConmgr()
{
	
	
}
CRtcConmgr::~CRtcConmgr()
{
	
	
}
int CRtcConmgr::AddRtcConn(std::string peerId,std::shared_ptr<erizo::WebRtcConnection> conn)
{
	std::map<std::string,std::shared_ptr<erizo::WebRtcConnection>>::iterator It=m_peerIdRtcConnMap.find(peerId);
	if(It!=m_peerIdRtcConnMap.end()){
		return 0;
	}else{
		m_peerIdRtcConnMap.insert(std::pair<std::string,std::shared_ptr<erizo::WebRtcConnection>>(peerId,conn));
	}
	return 1;
}