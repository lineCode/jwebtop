#ifndef jwebtop_task_H_
#define jwebtop_task_H_

#include <string>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>
namespace jw{
	namespace task{
		using namespace std;

		// ��������ִ�н����װ��
		class ProcessMsgLock
		{
		private:
			mutex lock;
			condition_variable g_queuecheck;
		public:
			wstring result;
			ProcessMsgLock(){}
			~ProcessMsgLock(){};
			void wait(){
				unique_lock<mutex> locker(lock);
				g_queuecheck.wait(locker);
			}
			void notify(){
				unique_lock<mutex> locker(lock);
				g_queuecheck.notify_all();
			}
		};

		// map<key,value>,key=�ѳɹ����͵�Զ�̵������id��value=����ִ����ɺ��������ִ�н��
		typedef map<wstring, ProcessMsgLock * > WaitTaskMap;

		// ���������id
		wstring createTaskId();

		// ���һ���ȴ����������б�
		ProcessMsgLock * addTask(wstring taskId);

		// �Ƴ�����ɵ�����
		void removeTask(wstring taskId);

		// ������ִ�н��������������������notify֪ͨ�ȴ��ĳ���һ����������һ���߳��е���
		bool putTaskResult(wstring taskId, wstring taskResult);
	}
}
#endif