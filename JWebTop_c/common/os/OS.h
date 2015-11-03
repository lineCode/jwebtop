#ifndef jwebtop_common_OS_H_
#define jwebtop_common_OS_H_

#include <string>

using namespace std;
namespace jw{
	namespace os{
		namespace file{			
			// �����ļ�����ȡ�ļ�����·��
			wstring GetFilePath(wstring appDefFile);

			// �õ���������Ŀ¼
			wstring GetExePath();

			// �������·����ȡ����·��
			// ���relativePath�Ѿ��Ǿ���·������ֱ�ӷ���
			// ���򽫷���appPath+relativePath
			wstring getAbsolutePath(wstring &relativePath, wstring &appPath);
		}// End file namespace
		namespace process{
			// ����һ���½��̣����ص�����Ϊ���������̵߳�id
			DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine);
		}// End process namespace
	}// End os namespace
}// End jw namespace
#endif