#include "IKKDirIo.h"
#ifdef _WINDOWS
#include "../tool/WinDir.h"
#include "../tool/Dir/Dir.hpp"
#endif
namespace kkBase
{
	IKKDirIo::IKKDirIo()
	{
	
	}
	IKKDirIo::~IKKDirIo()
	{
	
	}
	///获取当前模块目录
    std::string IKKDirIo::GetCurModulePath()
	{
	    #ifdef _WINDOWS
		       CWinDir dir;
			   return  dir.GetModulePathA();
		#else
		       return  "";
		#endif
	}
	void IKKDirIo::GetListFiles(std::list<std::string>& list,const std::string& folder,const std::string& extension,bool recursive)
	{
#ifdef _WINDOWS
		dir::listFiles(list,folder,extension,recursive);
#endif
	}
}