#ifndef CEF_JWEBTOP_WIN_CTRL_H_
#define CEF_JWEBTOP_WIN_CTRL_H_
#include <Windows.h>
#include <sstream>
#include <string>

namespace jw{
	void  setCenter(HWND hWnd);                             // setToCenter(handler);// �ô�����ʾ����Ļ����
	void  setCenter(HWND hWnd, int x, int y, int w, int h); // setToCenter(handler);// �ô�����ʾ����Ļ����
	POINT getPos(HWND hWnd);                                // getPos(handler);//��ô���λ�ã�����ֵΪһobject����ʽ����{x:13,y:54}
	void  setSize(HWND hWnd, int w, int h);                 // setSize(x, y, handler);//���ô��ڴ�С
	POINT getSize(HWND hWnd);                               // getSize(handler);//��ô��ڴ�С������ֵΪһobject����ʽ����{width:130,height:54}
	POINT getScreenSize();                                  // getScreenSize();//��ȡ��Ļ��С������ֵΪһobject����ʽ����{width:130,height:54}
	void  move(HWND hWnd, int x, int y);                    // move(x, y, handler);//�ƶ�����
	void  setBound(HWND hWnd, int x, int y, int w, int h);  // setBound(x, y,w ,h, handler);// ͬʱ���ô��ڵ�����ʹ�С
	RECT  getBound(HWND hWnd);                              // getBound(handler);//��ȡ������Ϣ������ֵΪ�����ϽǺ����½ǵ���Ļ����
	void  setTitle(HWND hWnd, std::wstring title);          // setTitle(title, handler);// ���ô�������
	std::wstring getTitle(HWND hWnd);                       // getTitle(handler);// ��ȡ�������ƣ�����ֵΪһ�ַ���

	void bringToTop(HWND hWnd);	// bringToTop(handler);//�����Ƶ����
	void focus(HWND hWnd);	    // focus(handler);//ʹ���ڻ�ý���
	void hide(HWND hWnd);	    // hide(handler);//���ش���
	void max(HWND hWnd);	    // max(handler);//��󻯴���
	void mini(HWND hWnd);	    // mini(hander);//��С������
	void restore(HWND hWnd);	// restore(handler);//��ԭ���ڣ���Ӧ��hide����
	BOOL isVisible(HWND hWnd);  // �����Ƿ�����ʾ
	void setTopMost(HWND hWnd);	// setTopMost(handler);//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����

	void setWindowStyle(HWND hWnd, int exStyle);	// setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
	void setWindowExStyle(HWND hWnd, int exStyle);  // setWindowExStyle(exStyle, handler)
}
#endif