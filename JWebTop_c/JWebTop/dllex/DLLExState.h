#ifndef CEF_JWEBTOP_EXE_DLLEX_STATE_H_
#define CEF_JWEBTOP_EXE_DLLEX_STATE_H_
#include <Windows.h>
#include <string>
#include <list>
#include <map>
#include "common/task/Task.h"
#include "include/cef_browser.h"

using namespace std;
namespace jw{
	namespace dllex{// ��DLL���н����������չ����
		class DLLExState{
		private:
			typedef list<task::ProcessMsgLock *> DefLockedTaskList;		// ����һ���洢ProcessMsgLock��List��
			typedef map<HWND, DLLExState * > DefBrowserSateMap;			// ����һ���洢�������չ״̬��Map��
			static DefBrowserSateMap browserStateMap;					// �����������չ״̬����̬������

			DefLockedTaskList taskList;									// ���浱ǰ��������Ѵ�������δ�⿪����
			wstring appendJS;											// �������Ҫ���ӵ�JS
			static DLLExState * findWithoutCreate(HWND browserHWnd);	// ���������״̬������Ҳ�������null
		public:
			void addLockedTask(task::ProcessMsgLock * lock);			// ���һ���µ�����������չ״̬
			void removeLockedTask(task::ProcessMsgLock * lock);			// �Ƴ�һ��������

			wstring getAppendJS();										// �õ���ǰ�������Ҫ���ӵ�JS	
			void setAppendJS(wstring js);								// ���õ�ǰ�������Ҫ���ӵ�JS

			static void unlockBrowserLocks(HWND browserHWnd);			// ������ǰ���е�������
			static DLLExState * findOrCreateExState(HWND browserHWnd);	// ���ҵ�ǰ�������չ״̬������Ҳ������Զ�������һ���л���ҳʱ���ã�
			static void removeBrowserSetting(HWND browserHwnd);			// ��������������չ״̬��һ��ر�ʱ���ã����˷������ȵ���unlockBrowserLocks����Ȼ�������
			static void appendBrowserJS(HWND browserHwnd, const CefRefPtr<CefFrame> frame);// ÿ��ҳ��OnLoad�󸽼�һЩJS

		};// End DLLExState class
	}// End dllex namespace
}// End jw namespace
#endif