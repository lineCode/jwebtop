#ifndef __MYUTIL_H__
#define __MYUTIL_H__
#include <xstring>
using namespace std;

wstring&   replace_allW(wstring&   str,const   wstring&   old_value,const   wstring&   new_value);
string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value);
string&   replace_all(string&   str,const   string&   old_value,const   string&   new_value);
bool isGB(const char*gb,int len);
int IsTextUTF8(const char* str,long length);
char* U2G(const char* utf8);
char* G2U(const char* gb2312);
char* B2U(const char* big5);

// ��charת��Ϊwchar_t
wchar_t* chr2wch(const char* tmp);
// ��charת��Ϊwchar_t
wchar_t* chr2wch(char* tmp);
// ��wchar_tת��Ϊchar
char* wch2chr(const wchar_t* wp);
// ��wchar_tת��Ϊchar
char* wch2chr(wchar_t* wp);

// ����Ψһ���ַ���
std::string GenerateGuidA();
std::wstring GenerateGuidW();
#endif
