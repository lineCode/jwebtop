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
#include <list>
#include <mutex>
#include "jwebtopex/jwebtopex.h"

#define BTN_ADD_NOTE		101
#define BTN_DEL_NOTE		102

#define MARGIN				10
#define BAR_HEIGHT			24
#define BTN_WIDTH			120

// �������ں���
void createWin(HINSTANCE hInstance);
void relayout();

class DemoCtrl :public JWebtopJSONDispater
	, public  JWebTopBrowserCreated
	, public JWebTopAppInited {
private:
	typedef list<wstring> NameList;
	NameList * names;
	JWebTopContext * ctx = NULL;
	HWND listBrowser = NULL;
	HWND detailBrowser = NULL;
	wstring currentNote;
	mutex lock;

	void _startJWebTop();
	void initNames();
	void showDetail(wstring note);
	void saveNotes();
	void saveNote(wstring note, wstring content);
	int getNoteIdx(wstring note){
		//return find(names->begin(), names->end(), note) != names->end();
		int idx = 0;
		NameList::iterator it;
		for (it = names->begin(); it != names->end(); it++){
			if (note == *(it))return idx;
			idx++;
		}
		return -1;
	}
	wstring getNoteIdx(int idx){
		NameList::iterator it;
		for (it = names->begin(); it != names->end(); it++){
			if (idx-- ==0)return * it;
			idx++;
		}
		return L"";
	}
public :
	void init();

	wstring getListAppFile();	// �õ������ļ�
	wstring getDetailAppFile();	// �õ������ļ�
	wstring getJWebTopExe();	// �õ�JWebTop.exe��·��
	HWND getListBrowser(){ return this->listBrowser; };
	HWND getDetailBrowser(){ return this->detailBrowser; };
	wstring addNote(wstring note);
	void delNote();

	void onJWebTopAppInited();
	void onJWebTopBrowserCreated(long browserHWnd);
	void onJWebTopJSReturn(wstring jsonString);
	wstring dispatcher(long browserHWnd, wstring json);

};