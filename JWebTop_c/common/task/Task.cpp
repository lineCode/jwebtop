#include "Task.h"
#include <sstream>
#include "common/util/StrUtil.h"

using namespace std;
namespace jw{
	namespace task{
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

		//long count = 0;// 
		// ����һ������id����ø�ΪUUID��
		wstring createTaskId(){
			return jw::GenerateGuidW();
			//mapLock.lock();
			//wstringstream wss;
			//count++;
			//wss << "id_" << count;
			//mapLock.unlock();
			//return wss.str();
		}

		// �������ִ�н��
		bool putTaskResult(wstring taskId, wstring taskResult){
			ProcessMsgLock * lock = getTask(taskId);
			if (lock == NULL)return false;
			lock->result = taskResult;
			lock->notify();
			return true;
		}

		// �������
		ProcessMsgLock * addTask(wstring taskId){
			ProcessMsgLock * lock = new ProcessMsgLock();
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
			WaitTaskMap::iterator it = WaitTasks.begin();
			while (WaitTasks.end() != it) {
				try{// ͨ��try-catch��֤ÿ�ν���ʧ�ܲ�Ӱ�������Ľ���
					it->second->notify();
				}catch (...){}
				++it;
			}
			WaitTasks.clear();
		}
	}
}