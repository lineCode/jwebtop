#include "Task.h"
#include <sstream>
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"


using namespace std;
namespace jw{
	namespace task{

		wstring ProcessMsgLock::wait(){
			if (notified)return result;
			unique_lock<mutex> locker(lock);
			g_queuecheck.wait(locker);
			removeTask(this->taskId);
			return result;
		}
		void ProcessMsgLock::notify(wstring result){
			this->result = result;
			unique_lock<mutex> locker(lock);
			g_queuecheck.notify_all();
			notified = true;
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