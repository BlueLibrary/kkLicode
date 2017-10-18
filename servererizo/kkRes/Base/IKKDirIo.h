#ifndef IKKDirIo_H_
#define IKKDirIo_H_
#include <string>
#include <list>
///文件及目录操作
namespace kkBase
{
	class IKKDirIo
	{
		public:
				IKKDirIo();
				~IKKDirIo();
				///获取当前模块目录
				std::string GetCurModulePath();
				///获取文件加下的全部文件
				void GetListFiles(std::list<std::string>& list,const std::string& folder,const std::string& extension = "",bool recursive = false);
	};
}
#endif