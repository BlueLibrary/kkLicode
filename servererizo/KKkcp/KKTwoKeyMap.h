#ifndef KKTwoKeyMap_H_
#define KKTwoKeyMap_H_
#include "kkRes/Base/KKRWLock.h"
#include <map>
template<
	class _Kty,
	class _Kty2,
	class _Ty
>
class TKKTwoKeyMap
{
public:
	TKKTwoKeyMap(void)  {  }
	~TKKTwoKeyMap(void)  {  }

	void insert( _Kty k1,  _Kty2 k2, _Ty value)
	{
		typename std::map<_Kty,_Ty>::iterator It1= m_Map1.find( k1);
		if(It1==m_Map1.end())
		{
			m_Map1.insert(std::pair<_Kty,_Ty>(k1,value));	
		}

		typename std::map<_Kty2,_Ty>::iterator It2= m_Map2.find( k2);
		if(It2==m_Map2.end())
		{
			m_Map2.insert(std::pair<_Kty2,_Ty>(k2,value));
		}
		
		typename std::map<_Ty,std::pair<_Kty,_Kty2> >::iterator It3=m_Map3.find(value);
		if(It3==m_Map3.end())
		{
			m_Map3.insert(std::pair<_Ty,std::pair<_Kty,_Kty2> >(value,std::pair<_Kty,_Kty2>(k1,k2)));
		}else{
			It3->second.first=k1;
			It3->second.second=k2;
		}
	}

	void Key1Insert( _Kty k1,   _Ty value)
	{
		typename std::map<_Kty,_Ty>::iterator It1= m_Map1.find( k1);
		if(It1==m_Map1.end())
		{
			m_Map1.insert(std::pair<_Kty,_Ty>(k1,value));	
		}

		typename std::map<_Ty,std::pair<_Kty,_Kty2> >::iterator It3=m_Map3.find(value);
		if(It3==m_Map3.end())
		{
			m_Map3.insert(std::pair<_Ty,std::pair<_Kty,_Kty2> >(value,std::pair<_Kty,_Kty2>(k1,0)));
		}else{
			It3->second.first=k1;
		}
	}
	int UpdateKey2( _Kty k1, _Kty2 k2)
	{
		typename std::map<_Kty,_Ty>::iterator It1= m_Map1.find( k1);
		if(It1==m_Map1.end()){
			return 0;
		}

		typename std::map<_Kty2,_Ty>::iterator It2= m_Map2.find( k2);
		if(It2==m_Map2.end()){
			m_Map2.insert(std::pair<_Kty2,_Ty>(k2,k1));
		}

		typename std::map<_Ty,std::pair<_Kty,_Kty2> >::iterator It3=m_Map3.find(It1->second);
		if(It3!=m_Map3.end())
		{
		     It2= m_Map2.find(It3->second.second);
			 if(It2!=m_Map2.end()){
				  m_Map2.erase(It2);
			 }
		}
		return 1;

	}
	void key2insert( _Kty k1,  _Kty2 k2, _Ty value)
	{
	
		typename std::map<_Kty2,_Ty>::iterator It2= m_Map2.find( k2);
		if(It2==m_Map2.end())
		{
			m_Map2.insert(std::pair<_Kty2,_Ty>(k2,value));
		}

		typename std::map<_Ty,std::pair<_Kty,_Kty2> >::iterator It3=m_Map3.find(value);
		if(It3==m_Map3.end())
		{
			m_Map3.insert(std::pair<_Ty,std::pair<_Kty,_Kty2> >(value,std::pair<_Kty,_Kty2>(k1,k2)));
		}else{
			It3->second.second=k2;
		}
	}
	typename std::map<_Kty,_Ty>::iterator Key1Del(_Kty k1)
	{
		_Kty2 *k2=NULL;
		int ret=0;
		////CKKRWLockGurd gurd(m_RLKcpMapLock,false);
		typename std::map<_Kty,_Ty>::iterator It1= m_Map1.find(k1);
		typename std::map<_Ty,std::pair<_Kty,_Kty2> >::iterator It3=m_Map3.begin();
		if(It1!=m_Map1.end()){
			It3=m_Map3.find(It1->second);
			if(It3!=m_Map3.end())			
				k2=&It3->second.second;
			m_Map1.erase(It1++);
		}
		if(k2!=NULL){
			typename std::map<_Kty2,_Ty>::iterator It2= m_Map2.find(*k2);
			if(It2!=m_Map2.end())
				m_Map2.erase(It2);
		}
		if(It3!=m_Map3.end())
			m_Map3.erase(It3);/**/
		return It1;
		//
	}
	typename std::map<_Kty,_Ty>::iterator Key1Find(_Kty k1)
	{
		return m_Map1.find(k1);
	}
	typename std::map<_Kty2,_Ty>::iterator Key2Find(_Kty2 k2)
	{
		return m_Map2.find(k2);
	}
	typename std::map<_Kty,_Ty>::iterator Key1begin()
	{
		return m_Map1.begin();
	}
	typename std::map<_Kty,_Ty>::iterator Key1End()
	{
		return m_Map1.end();
	}
	typename std::map<_Kty2,_Ty>::iterator Key2begin()
	{
		return m_Map2.begin();
	}
	typename std::map<_Kty2,_Ty>::iterator Key2End()
	{
		return m_Map2.end();
	}/**/
	void lock(bool readlock)
	{
		if(readlock)
		    m_RLKcpMapLock.Read();
		else
			m_RLKcpMapLock.Write();
	}
	void unlock(bool readlock)
	{
		if(readlock)
			m_RLKcpMapLock.Runlock();
		else
			m_RLKcpMapLock.Wunlock();
	}
protected:
	std::map<_Kty,_Ty>                     m_Map1;
	std::map<_Kty2,_Ty>                    m_Map2;
	std::map< _Ty, std::pair<_Kty, _Kty2> >    m_Map3;
	kkBase::CKKRWLock                      m_RLKcpMapLock;

};
#endif