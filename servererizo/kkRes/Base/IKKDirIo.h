#ifndef IKKDirIo_H_
#define IKKDirIo_H_
#include <string>
#include <list>
///�ļ���Ŀ¼����
namespace kkBase
{
	class IKKDirIo
	{
		public:
				IKKDirIo();
				~IKKDirIo();
				///��ȡ��ǰģ��Ŀ¼
				std::string GetCurModulePath();
				///��ȡ�ļ����µ�ȫ���ļ�
				void GetListFiles(std::list<std::string>& list,const std::string& folder,const std::string& extension = "",bool recursive = false);
	};
}
#endif