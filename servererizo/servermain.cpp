#include "stdafx.h"
#include "RtcConmgr.h"
///�̳߳�
std::unique_ptr<erizo::IOThreadPool> GIothreadPool;
///�ⲿ�������
std::shared_ptr<erizo::ThreadPool> GThreadPool;
static int num_workers=5;
int main(int argc,char *argv[])
{
	///�����߳�
	GIothreadPool.reset(new erizo::IOThreadPool(num_workers));
	GThreadPool.reset(new erizo::ThreadPool(num_workers));
	std::shared_ptr<erizo::Worker> worker = GThreadPool->getLessUsedWorker();
    std::shared_ptr<erizo::IOWorker> io_worker = GIothreadPool->getLessUsedIOWorker();
	return 0;
}