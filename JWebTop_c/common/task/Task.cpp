#include "Task.h"
#include <sstream>
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"


using namespace std;
namespace jw{
	namespace task{
		DWORD g_defaultWaitTime = 0;
		void setDefaultTaskWiatTime(DWORD defaultWaitTime){
			g_defaultWaitTime = defaultWaitTime;
		}
		wstring ProcessMsgLock::wait(){
			return wait(g_defaultWaitTime);
		}
		wstring ProcessMsgLock::wait(DWORD microseconds){
			if (!notified){						// ����Ѿ�֪ͨ����ֱ�ӷ���֪ͨ�Ľ��
				unique_lock<mutex> locker(lock);// �����������ϣ�����ʱ�Զ�������
				if (!notified){					// �������ټ����״̬λ������early notify
					if (microseconds > 0){		// �����ָ����ʱʱ�䣬��ôʹ�ó�ʱ�ȴ���ʽ
						g_queuecheck.wait_until(locker, chrono::system_clock::now() + chrono::microseconds(microseconds));
					}
					else{						// ����һֱ�ȴ���ֱ������notify�ĵ���
						g_queuecheck.wait(locker);
					}
				}
			}
			removeTask(this->taskId);			// �Ƴ������������
			return this->result;
		}
		void ProcessMsgLock::notify(wstring result){
			this->result = result;				// ������������ִ�н��
			notified = true;					// ����֮ǰ�Ȱѱ�־λ���ˣ���Ϊ��ʱ���������
			unique_lock<mutex> locker(lock);	// �����������ϣ�����ʱ�Զ�������
			g_queuecheck.notify_all();			// ֪ͨ���еȴ��������������
		}

		WaitTaskMap WaitTasks; // ���������Ѵ�����������Ϣ	
		mutex mapLock;         // �������map�Ĳ�����Ҫ���� 
		ProcessMsgLock * getTask(wstring taskId){
			ProcessMsgLock * rtn = NULL;
			mapLock.lock();
			try{
				WaitTaskMap::iterator it = WaitTasks.find(taskId);
				if (WaitTasks.end() != it) {
					//it->second->wait();// findʱ����ȴ�
					rtn = it->second;
				}
				mapLock.unlock();
			}
			catch (...){
				mapLock.unlock();
			}
			return rtn;
		}

		// ����һ������id����ø�ΪUUID��
		wstring createTaskId(){
			return jw::GenerateGuidW();
		}

		// �������ִ�н��
		bool putTaskResult(wstring taskId, wstring taskResult){
			ProcessMsgLock * lock = getTask(taskId);
			if (lock == NULL)return false;
			lock->notify(taskResult);
			return true;
		}

		// �������
		ProcessMsgLock * addTask(wstring taskId){
			ProcessMsgLock * lock = new ProcessMsgLock(taskId);
			mapLock.lock();
			try{
				pair<WaitTaskMap::iterator, bool> v = WaitTasks.insert(pair<wstring, ProcessMsgLock * >(taskId, lock));// ��¼������id�����Ĺ���
				mapLock.unlock();
			}
			catch (...){
				mapLock.unlock();
				lock = NULL;
			}
			return  lock;
		}

		// �Ƴ�����
		void removeTask(wstring taskId){
			mapLock.lock();
			try{
				WaitTasks.erase(taskId);
				mapLock.unlock();
			}
			catch (...){
				mapLock.unlock();
			}
		}

		// �������������������˷������ҽ����ڽ����˳�ʱִ��
		void unlockAndClearAll(){
#ifdef JWebTopLog
			wstringstream wss;
			wss << L"����δ���������=";
			wss << WaitTasks.size();
			wss << L"\r\n";
			writeLog(wss.str());
#endif
			WaitTaskMap::iterator it = WaitTasks.begin();
			while (WaitTasks.end() != it) {
				try{// ͨ��try-catch��֤ÿ�ν���ʧ�ܲ�Ӱ�������Ľ���
#ifdef JWebTopLog
					wss.clear();
					wss << L"\t"
						<< " taskId=" << it->first << "\r\n";
					writeLog(wss.str());
#endif
					it->second->notify(L"");
				}
				catch (...){}
				++it;
			}
			WaitTasks.clear();
		}
	}
}