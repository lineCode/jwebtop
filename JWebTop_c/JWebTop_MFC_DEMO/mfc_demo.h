//////////////////////////////////////////////////////////////////////////  
/// �������fatipc��������ͻ���ͨ��ʱ��Ҫ�Ĵ��ڡ��������Լ�������
///  
/// Դ�룺https://github.com/washheart/jwebtop
/// ˵����https://github.com/washheart/jwebtop/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <Windows.h>
#include <string>
#include <sstream>

#define BTN_SEND			101
#define BTN_CLEAR_SEND		102
#define BTN_CLEAR_RECEIVE	103

#define TXT_Msg				201
#define TXT_Sended			202
#define TXT_Received		203

#define MARGIN				10
#define BAR_HEIGHT			24
#define BTN_WIDTH			120

int startIPCServer();
void _onRead();


// ���·���ʵ���ڡ�wincomm.cpp���ļ�-----------------------------------------------
// ��ȡ���ڵı���
LPTSTR getWindowTitle();
// �������ں���
BOOL createWin(HINSTANCE hInstance);
// �ڽ�����Ϣ���ı�����׷���յ���Ϣ
void appendReceied(std::wstring received);
// �ڷ�����Ϣ���ı�����׷�ӷ����˵���Ϣ
void appendSended(std::wstring sended);


// ���·���ʵ���ڡ�ipccom.cpp���ļ�------------------------------------------------
// ������Ϣ
DWORD sendMsg(std::wstring msg);
// ���ڴ��ڴ�����ִ��������������������������
void windowCreated(HWND hWnd);