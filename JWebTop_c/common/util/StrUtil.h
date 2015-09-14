#ifndef __MYUTIL_H__
#define __MYUTIL_H__
#include <xstring>
#include <windows.h>
using namespace std;

namespace jw{
	wstring&   replace_allW(wstring& str, const wstring& old_value, const wstring& new_value);
	string&    replace_all (string&  str, const string&  old_value, const string&  new_value);
	//string&   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value);

	//wstring&   replace_all(wstring&   str, const   wstring&   old_value, const   wstring&   new_value);
	//bool isGB(const char*gb, int len);
	//int IsTextUTF8(const char* str, long length);
	//char* U2G(const char* utf8);
	//char* G2U(const char* gb2312);
	//char* B2U(const char* big5);

	// ������ĸ������ᵼ���ڴ�й©��Ӧ�ð�xxx(yyy * from,zzz * to)�ķ�ʽ��д����������������ȡֵ�������ڴ�й©
	//// ��charת��Ϊwchar_t
	//wchar_t* chr2wch(const char* tmp);
	//// ��charת��Ϊwchar_t
	//wchar_t* chr2wch(char* tmp);
	//// ��wchar_tת��Ϊchar
	//char* wch2chr(const wchar_t* wp);
	//// ��wchar_tת��Ϊchar
	//char* wch2chr(wchar_t* wp);

	string w2s(const wstring &ws);
	wstring s2w(const string &s);

	// ����Ψһ���ַ���
	std::string GenerateGuidA();
	std::wstring GenerateGuidW();

	// ��ȡ��ǰ�����ں�ʱ�䣬��ʽ��yyyy-mm-dd hh:MM:ss
	std::string getDateTime();

	long parseLong64(const wstring &ws);
	long parseLong(const wstring &ws);
	int parseInt(const wstring &ws);

	// ��ȡ�ļ����ݵ�result�У�append��ʽ������ȡ�ɹ�����1��ʧ�ܷ���0
	int readfile(wstring file, string &result);
}
#endif