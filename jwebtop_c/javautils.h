
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <iostream>


// ��charת��Ϊwchar_t
wchar_t* chr2wch(const char* tmp)
{
	size_t len = strlen(tmp);
	size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), NULL, 0);
	wchar_t*  buffer = new wchar_t[wlen];
	MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), buffer, int(wlen));
	buffer[wlen] = 0;
	return buffer;
}
// ��charת��Ϊwchar_t
wchar_t* chr2wch(char* tmp)
{
	size_t len = strlen(tmp);
	size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), NULL, 0);
	wchar_t*  buffer = new wchar_t[wlen];
	MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), buffer, int(wlen));
	buffer[wlen] = 0;
	return buffer;
}

// ��wchar_tת��Ϊchar
char* wch2chr(const wchar_t* wp)
{
	size_t wlen = wcslen(wp);
	size_t len = WideCharToMultiByte(CP_ACP, 0, wp, wlen, NULL, 0, NULL, NULL);
	char *m_char = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wp, wlen, m_char, len, NULL, NULL);
	m_char[len] = 0;
	return m_char;
}
// ��wchar_tת��Ϊchar
char* wch2chr(wchar_t* wp)
{
	size_t wlen = wcslen(wp);
	size_t len = WideCharToMultiByte(CP_ACP, 0, wp, wlen, NULL, 0, NULL, NULL);
	char *m_char = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wp, wlen, m_char, len, NULL, NULL);
	m_char[len] = 0;
	return m_char;
}

//��java���ַ���ת����windows���ַ���   
char*   jstringToWindows(JNIEnv*   env, jstring   jstr)
{
	int   length = env->GetStringLength(jstr);
	const   jchar*   jcstr = env->GetStringChars(jstr, 0);
	char*   rtn = (char*)malloc(length * 2 + 1);
	int   size = 0;
	size = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)jcstr, length, rtn,
		(length * 2 + 1), NULL, NULL);
	if (size <= 0)
		return   NULL;
	env->ReleaseStringChars(jstr, jcstr);
	rtn[size] = 0;
	return   rtn;
}