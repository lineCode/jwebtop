#ifndef jwebtop_multi_process_H_
#define jwebtop_multi_process_H_

#include <Windows.h>

#define WM_COPYDATA_MINI  1     // ����WM_COPYDATA��Ϣ��dwData����ֵ����ʾ��С����
#define WM_COPYDATA_LARGE 2     // ����WM_COPYDATA��Ϣ��dwData����ֵ����ʾ�Ǵ�����
#define WM_COPYDATA_HWND  9     // ����WM_COPYDATA��Ϣ��dwData����ֵ����ʾ���ڷ��ʹ���HAWND����һ�����̣���ʱ��С���ݣ�
#define MPMSG_USER        100   // ����MPMSG_MINI��MPMSG_LARGE��msgId����ʼֵ
#define MPMSG_MINI_LEN 1024     // ����С���ݽṹʱ�ַ����ĳ���
#define MPMSG_LARGE_LEN 65535   // ��������ݽṹʱ�ַ����ĳ���

typedef struct tagMPMSG_MINI
{
	TCHAR msg[MPMSG_MINI_LEN];
	DWORD msgId;
} MPMSG_MINI;

typedef struct tagMPMSG_LARGE
{
	TCHAR msg[MPMSG_LARGE_LEN];
	DWORD msgId;
} MPMSG_LARGE;

// �����½��̲����ؽ����ж�Ӧ���߳�id������ͨ��PostThreadMessage����֪ͨ����Ϣ��
DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine);

// ���Ϳ������Ϣ
// hWnd  :������Ϣ�Ĵ��ڵľ��
// msgId :����MPMSG_USER����ֵ
// msg   :�����͵��ַ�����Ϣ
BOOL sendProcessMsg(HWND hWnd, DWORD msgId, LPTSTR msg);

#endif