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

	// 下面的四个方法会导致内存泄漏，应该按xxx(yyy * from,zzz * to)的方式改写，即传入引用来获取值，避免内存泄漏
	//// 将char转换为wchar_t
	//wchar_t* chr2wch(const char* tmp);
	//// 将char转换为wchar_t
	//wchar_t* chr2wch(char* tmp);
	//// 将wchar_t转换为char
	//char* wch2chr(const wchar_t* wp);
	//// 将wchar_t转换为char
	//char* wch2chr(wchar_t* wp);

	string w2s(const wstring &ws);
	wstring s2w(const string &s);

	// 生成唯一性字符串
	std::string GenerateGuidA();
	std::wstring GenerateGuidW();

	// 获取当前的日期和时间，格式：yyyy-mm-dd hh:MM:ss
	std::string getDateTime();

	long parseLong64(const wstring &ws);
	long parseLong(const wstring &ws);
	int parseInt(const wstring &ws);

	// 读取文件内容到result中（append方式），读取成功返回1，失败返回0
	int readfile(wstring file, string &result);
}
#endif