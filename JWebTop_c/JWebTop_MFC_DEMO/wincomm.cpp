/// �����Լ��ռ䲼�ֵ���صķ����ͱ��������漰fastipc��صĵ���

#include "mfc_demo.h"

static TCHAR szAppName[] = TEXT("���Դ�������");
HWND btnSend, txtMsg, txtSend, txtReceive;
std::wstringstream sended, received;
LONG oldTxtMsgWndProc = NULL;
// �����¼�������
LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TxtMsg_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL __createWin(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindow(szAppName, getWindowTitle(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)return FALSE;
	int x = MARGIN;
	HWND btnClearSend = CreateWindow(TEXT("button"),//����Ϊ��button    
		TEXT("��շ���"),//��ť����ʾ���ַ�    
		WS_CHILD | WS_VISIBLE, x, 10, BTN_WIDTH, BAR_HEIGHT, hWnd,
		(HMENU)BTN_CLEAR_SEND, hInstance, NULL);
	x = x + BTN_WIDTH + MARGIN;
	HWND btnClearReceive = CreateWindow(TEXT("button"),//����Ϊ��button    
		TEXT("��ս���"),//��ť����ʾ���ַ�    
		WS_CHILD | WS_VISIBLE, x, 10, BTN_WIDTH, BAR_HEIGHT, hWnd,
		(HMENU)BTN_CLEAR_RECEIVE, hInstance, NULL);

	btnSend = CreateWindow(TEXT("button"),//����Ϊ��button    
		TEXT("������Ϣ"),//��ť����ʾ���ַ�    
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd,
		(HMENU)BTN_SEND, hInstance, NULL);
	txtMsg = CreateWindow(TEXT("edit"),//����Ϊ��button    
		TEXT("�����"),//��ť����ʾ���ַ�   
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_WANTRETURN,
		0, 0, 0, 0, hWnd,
		(HMENU)TXT_Msg, hInstance, NULL);

	txtSend = CreateWindow(TEXT("edit"),//����Ϊ��button    
		TEXT("�����"),//��ť����ʾ���ַ�   
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		0, 0, 0, 0, hWnd,
		(HMENU)TXT_Sended, hInstance, NULL);
	txtReceive = CreateWindow(TEXT("edit"),
		TEXT("�����"),//��ť����ʾ���ַ�    
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		0, 0, 0, 0, hWnd,
		(HMENU)TXT_Received, hInstance, NULL);

	HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	if (!hFont){
		LOGFONT font;
		ZeroMemory(&font, sizeof(font));
		lstrcpy(font.lfFaceName, L"����");
		font.lfHeight = 12;
		hFont = CreateFontIndirect(&font);
	}
	if (hFont){
		SendMessage(hWnd, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(btnSend, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(btnClearSend, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(btnClearReceive, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(txtMsg, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(txtSend, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(txtReceive, WM_SETFONT, WPARAM(hFont), TRUE);
	}
	SetWindowText(txtMsg, L"����������Ҫ���͵���Ϣ");
	SetWindowText(txtSend, L"��������ʾ�ѷ��͵���Ϣ");
	SetWindowText(txtReceive, L"��������ʾ���յ�����Ϣ");
	oldTxtMsgWndProc = GetWindowLongPtr(txtMsg, GWLP_WNDPROC);
	SetWindowLongPtr(txtMsg, GWLP_WNDPROC, (LONG)TxtMsg_WndProc);// �滻�����ڵ���Ϣ������

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void relayout(HWND hWnd){
	RECT	rect;
	GetClientRect(hWnd, &rect);
	int w = rect.right - MARGIN * 2;
	MoveWindow(btnSend, w - BTN_WIDTH + MARGIN / 2, MARGIN, BTN_WIDTH, BAR_HEIGHT, TRUE);
	MoveWindow(txtMsg, (MARGIN + BTN_WIDTH) * 2 + MARGIN, MARGIN, w - (BTN_WIDTH + MARGIN) * 3, BAR_HEIGHT, TRUE);
	int y = MARGIN + BAR_HEIGHT + MARGIN;
	int h = (rect.bottom - y - MARGIN/*�����ı���֮��ķָ�*/ - MARGIN/*�����ı����봰�ڱ߿��ķָ�*/) / 2;
	if (h < BAR_HEIGHT)h = BAR_HEIGHT;
	MoveWindow(txtSend, MARGIN, y, w, h, TRUE);
	MoveWindow(txtReceive, MARGIN, y + h + MARGIN, w, h, TRUE);
}

void appendReceied(std::wstring ws){
	received << L"���� " << ws << L"\r\n";
	SetWindowText(txtReceive, LPTSTR(received.str().c_str()));
	int nScrollCount = SendMessage(txtReceive, EM_GETLINECOUNT, 0, 0);
	SendMessage(txtReceive, EM_LINESCROLL, 0, nScrollCount);
}

void appendSended(std::wstring ws){
	sended << L"���� " << ws << L"\r\n";
	SetWindowText(txtSend, LPTSTR(sended.str().c_str()));
	int nScrollCount = SendMessage(txtSend, EM_GETLINECOUNT, 0, 0);
	SendMessage(txtSend, EM_LINESCROLL, 0, nScrollCount);
}
void __SendMsg(){
	int len = SendMessage(txtMsg, WM_GETTEXTLENGTH, 0, 0) + 1;
	wchar_t* txt = new wchar_t[len];
	GetWindowText(txtMsg, txt, len);
	std::wstring ws;
	ws.append(txt);
	delete[] txt;
	if (0 == sendMsg(ws)){
		appendSended(ws);
	}
}
void __clearSended(){
	sended.str(L"");
	SetWindowText(txtSend, L"");
}
void __clearReciveed(){
	received.str(L"");
	SetWindowText(txtReceive, L"");
}
// ��������
int createWin(HINSTANCE hInstance){
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = SimpleWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass)){
		MessageBox(NULL, TEXT("�޷��������ڣ�����ϵͳ�汾���ͣ�"), szAppName, MB_ICONERROR);
		return 0;
	}
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	return __createWin(hInstance);
}

LRESULT CALLBACK TxtMsg_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN)__SendMsg();
		break;
	}
	return CallWindowProc((WNDPROC)oldTxtMsgWndProc, hWnd, message, wParam, lParam);
}
LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		relayout(hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case BTN_SEND:__SendMsg(); break;
		case BTN_CLEAR_SEND:__clearSended(); break;
		case BTN_CLEAR_RECEIVE:__clearReciveed(); break;
		}
		break;
	case WM_CREATE:
		windowCreated(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}