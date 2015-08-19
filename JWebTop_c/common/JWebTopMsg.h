#ifndef jwebtop_multi_process_jwebtop_msg_H_
#define jwebtop_multi_process_jwebtop_msg_H_
#include <string>

#include "include/cef_parser.h"
#include "common/process/MultiProcess.h"

#define JWEBTOP_MSG_SUCCESS         (WM_USER+321)	    // ��ʾ��Ϣ�ɹ������ս����յ�


// JWM=JWEBTOP_MSG
#define JWM_JS_EXECUTE_WAIT		(MPMSG_USER+201)  // DLL����CEF�ˣ���Ҫִ�в��ȴ���JS���� 
#define JWM_JS_EXECUTE_RETURN	(MPMSG_USER+202)  // 
#define JWM_JSON_EXECUTE_WAIT	(MPMSG_USER+211)  // DLL����CEF�ˣ���Ҫִ�в��ȴ���JSON���� 
#define JWM_JSON_EXECUTE_RETURN	(MPMSG_USER+212)  // 
#define JWM_DLL_EXECUTE_WAIT	(MPMSG_USER+221)  // CEF����DLL�ˣ���Ҫִ�в��ȴ������� 
#define JWM_DLL_EXECUTE_RETURN	(MPMSG_USER+222)  // CEF����DLL�ˣ���Ҫִ�е�CEF����ȴ������� 
#define JWM_RESULT_RETURN	    (MPMSG_USER+231)  
#define JWM_STARTJWEBTOP		(MPMSG_USER+301)  // ����JWebTop���̵���Ϣ
#define JWM_CREATEBROWSER		(MPMSG_USER+302)  // �������������Ϣ
namespace jw{
	// �Ӵ����JSON�ַ���(jsonString)�н���������id(taskId)�;����JSON�ַ���(parsedJO)���Ѿ��ظ���Ϣ�Ĵ��ھ��
	// jsonString�Ľṹ���£�{"id":"task id string","hWnd":123456,"jo":"parsed json string"}
	// hWnd��taskId��parsedJsonStringΪ���������ֱֵ�ӷ��ء������ɹ��󷵻�true������ʧ�ܻ᷵��false
	bool parseMessageJSON(std::wstring jsonString,long &hWnd, std::wstring &taskId, std::wstring &parsedJsonString);

	// ������Ĵ�������Ϣ�Ĵ��ھ��������id(taskId)��JSON�ַ���(someJsonString)��װΪ�µ�json�ַ���
	// ����json�Ľṹ���£�{"id":"task id string","hWnd":123456,"jo":"some json string"}
	// taskId��someJsonStringʹ�õ��Ǵ��������ʽ��Ŀ��ֻ�Ǽ������ܿ������ڷ����в�Ӧ�޸���ֵ
	std::wstring wrapAsTaskJSON(long hWnd, std::wstring &taskId, std::wstring &someJsonString);
}
#endif