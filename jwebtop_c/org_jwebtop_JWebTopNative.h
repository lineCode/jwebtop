/* DO NOT EDIT THIS FILE - it is machine generated */
#include "jni_jdk1_6_24\jni.h"
#include "jni_jdk1_6_24\jni_md.h"
#include "jni_jdk1_6_24\jawt_md.h"
/* Header for class org_jwebtop_JWebTopNative */

#ifndef _Included_org_jwebtop_JWebTopNative
#define _Included_org_jwebtop_JWebTopNative
#ifdef __cplusplus
extern "C" {
#endif

 // dll��ں���
 BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved);
 
/*
 * ��Ӧorg.jwebtop.JWebTopNative���createJWebTop����
 * �÷������ڴ���һ�����������
 * appfile ��������ݴ������ļ����г�ʼ��
 * parentWin ������������ĸ��������ĸ�
 */
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_createJWebTop
(JNIEnv *, jclass, jstring appfile, jlong parentWin);

/*
 * ��Ӧorg.jwebtop.JWebTopNative���executeJs����
 * �÷�������ִ�������js�ű�������cef����ֱ�ӷ��ؽű�ֵ�����Է���ֵͨ���ص�java��ʽȥ����
 * browserHWnd Ҫִ��js���������browserHWnd�������������ͨ���ص���ʽ���ݸ�java����    
 * json ��Ҫִ�е�js�ű���������һ��json�ַ���
 */
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_executeJs
  (JNIEnv *, jclass,jlong browserHWnd, jstring json);

#ifdef __cplusplus
}
#endif
#endif