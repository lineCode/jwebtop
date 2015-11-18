#include "DLLExState.h"

#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif 

namespace jw{
	namespace dllex{// ��DLL���н����������չ����
		// ��¼��������
		void DLLExState::addLockedTask(jw::task::ProcessMsgLock * lock){
			taskList.push_back(lock);
		}

		// �Ƴ�һ��������
		void DLLExState::removeLockedTask(jw::task::ProcessMsgLock * lock){
			if (taskList.empty())return;
			DefLockedTaskList::iterator it = find(taskList.begin(), taskList.end(), lock);
			if (it != taskList.end()){
				taskList.erase(it);
			}
		}
		wstring DLLExState::getAppendJS(){
			return this->appendJS;
		}
		void DLLExState::setAppendJS(wstring js){
			this->appendJS = js;// ��¼�����������븽��JS�Ĺ���			
		}		

		// ��̬�����뷽��
		DLLExState::DefBrowserSateMap DLLExState::browserStateMap;					// �����������չ״̬����̬������

		DLLExState * DLLExState::findWithoutCreate(HWND browserHWnd){
			DLLExState * rtn = NULL;
			DefBrowserSateMap::iterator it = browserStateMap.find(browserHWnd);
			if (browserStateMap.end() != it) {
				rtn = it->second;
			}
			return rtn;
		}
		DLLExState * DLLExState::findOrCreateExState(HWND browserHWnd){
			DLLExState * rtn = NULL;
			DefBrowserSateMap::iterator it = browserStateMap.find(browserHWnd);
			if (browserStateMap.end() != it) {
				rtn = it->second;
			}
			else{
				rtn = new DLLExState();
				browserStateMap.insert(pair<HWND, DLLExState* >(browserHWnd, rtn));// ��¼��������������չ״̬�Ĺ���
			}
			return rtn;
		}

		void DLLExState::unlockBrowserLocks(HWND browserHwnd){
			DLLExState *  exState = findWithoutCreate(browserHwnd);
			if (exState == NULL)return;
			if (exState->taskList.empty())return;
			DefLockedTaskList tmpTasks(exState->taskList.begin(), exState->taskList.end());
			exState->taskList.clear();
			DefLockedTaskList::iterator it = tmpTasks.begin();
			for (; it != tmpTasks.end(); it++){
				(*it)->notify(L"");
			}
		}
		void DLLExState::removeBrowserSetting(HWND browserHwnd){
			DLLExState *  exState = findWithoutCreate(browserHwnd);
			if (exState == NULL)return;
			unlockBrowserLocks(browserHwnd);
			browserStateMap.erase(browserHwnd);
		}

		void DLLExState::appendBrowserJS(HWND browserHwnd, const CefRefPtr<CefFrame> frame){
			DLLExState *  exState = findWithoutCreate(browserHwnd);
			if (exState == NULL)return;
			wstring js = exState->getAppendJS();
#ifdef JWebTopLog
			writeLog(L"���ӵ�JS====="); writeLog(js); writeLog(L"\r\n");
#endif
			if (!js.empty())frame->ExecuteJavaScript(js, "", 0);
		}
	}// End dllex namespace
}// End jw namespace