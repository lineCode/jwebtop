#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
using namespace std;
namespace jw{
	namespace dllex{// ��DLL���н����������չ����
		// �Ƿ���dll��չ
		bool ex();
		
		// ����������Ѵ�����Ϣ���ͻ���
		void sendBrowserCreatedMessage(wstring taskId,long browserHWnd);

		// ����һ���߳���������Զ�̽����Ƿ���ֹ�Ա������ǰ����
		void waitForRemoteProcessTerminate();

		// ����������������ڵ�WM_COPYDATA��Ϣ
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	}
}
#endif