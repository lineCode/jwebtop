#ifndef jwebtop_task_H_
#define jwebtop_task_H_

#include <string>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>

using namespace std;
namespace jw{
	namespace task{
		//template  class __declspec(dllexport) std::allocator<wchar_t>;
		//template  class __declspec(dllexport) std::map < std::string, ProcessMsgLock* >;
		// ��������ִ�н����װ��
		class /*_declspec(dllexport)*/ ProcessMsgLock
		{
		private:
			mutex lock;
			condition_variable g_queuecheck;
			bool notified = false;	// ��������Ƿ���֪ͨ������ֹ����֪ͨ�ڵȴ�֮ǰ�ͷ�����ҲΪ���ڶ�������ʱ���н�����
			wstring result;			// �����ִ�н����������js��ִ�н����Ҳ�����������߳�����ȣ�
			wstring taskId;			// �����Ψһ��ʶ���������uuid
		public:
			ProcessMsgLock(wstring taskId){ this->taskId = taskId; }
			~ProcessMsgLock(){ if (!notified)notify(L""); };
			wstring wait();					 // �ȴ�ִ�н������defaultWaitTime���еȴ�
			wstring wait(DWORD microseconds);// �ȴ�ִ�н��
			void notify(wstring result);	 // ����ִ�н��
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

		// �������������������˷������ҽ����ڽ����˳�ʱִ��
		void unlockAndClearAll();

		// ���������Ĭ�ϵȴ�ʱ�䣬������������еĵȴ������һֱ����ȥ���������������
		void setDefaultTaskWiatTime(DWORD defaultWaitTime);
	}
}
#endif