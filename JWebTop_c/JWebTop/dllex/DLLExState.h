#ifndef CEF_JWEBTOP_EXE_DLLEX_STATE_H_
#define CEF_JWEBTOP_EXE_DLLEX_STATE_H_
#include <Windows.h>
#include <string>
#include <list>
#include <map>
#include "include/cef_browser.h"

using namespace std;
namespace jw{
	namespace dllex{// ��DLL���н����������չ����
		class DLLExState{
		private:
			typedef map<wstring, wstring> DefBrowserCallBackMap;		// ����һ���洢�����������ص�������Map��
			typedef map<HWND, DLLExState * > DefBrowserSateMap;			// ����һ���洢�������չ״̬��Map��
			static DefBrowserSateMap browserStateMap;					// �����������չ״̬����̬������

			DefBrowserCallBackMap callBacks;
			wstring appendJS;											// �������Ҫ���ӵ�JS
			static DLLExState * findWithoutCreate(HWND browserHWnd);	// ���������״̬������Ҳ�������null
		public:

			void addCallBack(wstring taskId, wstring callBack);			// ���һ����ص�������ص��������Ƽ�Ķ�Ӧ
			void removeCallBack(wstring taskId);						// �Ƴ�һ��ص���Ӧ

			wstring getAppendJS();										// �õ���ǰ�������Ҫ���ӵ�JS	
			void setAppendJS(wstring js);								// ���õ�ǰ�������Ҫ���ӵ�JS

			static void unlockBrowserLocks(HWND browserHWnd);			// ������ǰ���е�������
			static DLLExState * findOrCreateExState(HWND browserHWnd);	// ���ҵ�ǰ�������չ״̬������Ҳ������Զ�������һ���л���ҳʱ���ã�
			static void removeBrowserSetting(HWND browserHwnd);			// ��������������չ״̬��һ��ر�ʱ���ã����˷������ȵ���unlockBrowserLocks����Ȼ�������
			static void appendBrowserJS(HWND browserHwnd, const CefRefPtr<CefFrame> frame);// ÿ��ҳ��OnLoad�󸽼�һЩJS
			static wstring findAndRemoveCallBack(HWND browserHwnd, wstring taskId);
		};// End DLLExState class
	}// End dllex namespace
}// End jw namespace
#endif