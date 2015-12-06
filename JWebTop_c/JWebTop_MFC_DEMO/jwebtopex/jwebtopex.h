#ifndef CEF_JWEBTOP_ex_H_
#define CEF_JWEBTOP_ex_H_
#pragma once
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "JWebTopConfigs.h"
#include <thread>
#include <string>
#include <sstream>
//class JWebTopAppInited();

/**
 * ��JWebTop���̳ɹ���������ã�ֻ���ڴ�ʱ���֮�󣬲ſ��Թ��������
 */
typedef void onJWebTopAppInited();
/**
 * ��ָ������������������
 * @param browserHWnd �ɹ�������������Ĵ��ھ��
 */
typedef void onJWebTopBrowserCreated(long browserHWnd);
/**
* jsִ�к��ִ�н��
* @param uuid       ִ��JSʱ���ݵ�uuid�����ݴ���Ҫ����һ��JWebTopJSReturn�д�����JS���ؽ�������
* @param jsonString ִ�н������������������һ��JSON�ַ���
*/
typedef void onJWebTopJSReturn(wstring uuid, wstring jsonString);


/**
* ���������������json���ݣ������ؽ��
*
* @param browserHWnd
*            ��Ҫִ��json����������
* @param json
*            ������˷�����Ҫִ������
* @return ���ش����Ľ��
*/
typedef wstring onDispatcher(long browserHWnd, wstring json);
class JWebTopEx
{
public:
	std::wstring serverName;	// ���������
	DWORD blockSize;			// һ�δ���ʱ�����ݴ�С

	fastipc::Server* server;	// IPC�����
	fastipc::Client* client;	// IPC�ͻ���

	onDispatcher* jsonHandler;	// ��Ӧ��������̷�����JSON���ݲ����д���

	JWebTopEx();
	~JWebTopEx(){ closeContext(); }

	/**
	 * ����JWebTop���̣��ɹ���ص�onJWebTopAppInited������
	 *
	 * @param processPath
	 * @param cfgFile
	 * @param appInitListner
	 */
	void createJWebTopByCfgFile(wstring processPath, wstring cfgFile, onJWebTopAppInited* appInitListner);

	/**
	 * ���������ļ�������������ɹ���ص�onJWebTopBrowserCreated������
	 *
	 * @param browserCfgFile
	 */
	void createBrowserByFile(wstring browserCfgFile, onJWebTopBrowserCreated* createListener);

	/**
	* �������ö�����������������ɹ���ص�onJWebTopBrowserCreated������
	* @param configs  ����������Ķ���
	*/
	void createBrowser(JWebTopConfigs configs, onJWebTopBrowserCreated* createListener);

	/**
	* ����JSON�����ַ���������������ɹ���ص�onJWebTopBrowserCreated������
	* @param browerCfgJSON  JSON�����ַ���
	*/
	void createBrowserByJSON(wstring browerCfgJSON, onJWebTopBrowserCreated* createListener);

	// �ر�ָ�������
	void closeBrowser(long browserHWnd);

	// �˳�JWebTop����
	void ExitJWebTop();

	// �رյ�ǰContext���е�FastIPC����ˡ��ͻ��ˣ��Լ����������
	void closeContext();

	/**
	* ����һ������ҳ��
	*
	* @param url
	*/
	void setErrorUrl(String url) {
		client.write(JWM_SET_ERR_URL, 0, null, url);
	}

	bool isStable() {
		return server != null && server.isStable();
	}

	void setUrl(long browserHWnd, wstring url) {
		client.write(JWM_M_SETURL, browserHWnd, null, url);
	}

	void setAppendJS(long browserHWnd, wstring js) {
		client.write(JWM_M_APPEND_JS, browserHWnd, null, js);
	}

	/**
	* ִ��js�����ͺ�ȴ����ݵķ��ء�<br/>
	* ���﷢���ǰ�װ��JSON���ݣ������JS�ű��е�invokeByDLL(jsonvalue)����
	*
	* @param browserHWnd
	*            ���ĸ������ִ��
	* @param jsonstring
	*            ��ִ�е�JSON����
	* @param jsReturn
	*            JSִ�н����������
	* @param uuid
	*            ִ��jsʱ���ڻ�ȡ���ؽ�������jsReturn�ǹ��õģ���ô���Ը��ݴ�uuid�����ֲ�ͬ�ĵ���
	*/
	void executeJSON_Wait(long browserHWnd, String jsonstring, JWebTopJSReturn jsReturn, String uuid) {
		String taskId = (uuid == null || uuid.trim().length() == 0) ? createTaskId() : uuid;
		jsReturnListners.put(taskId, jsReturn);
		client.write(JWM_JSON_EXECUTE_WAIT, browserHWnd, taskId, jsonstring);
	}

	/**
	* ִ��js�������ȴ����ݵķ��ء�<br/>
	* ���﷢���ǰ�װ��JSON���ݣ������JS�ű��е�invokeByDLL(jsonvalue)����
	*
	* @param jsonstring
	*/
	void executeJSON_NoWait(long browserHWnd, String jsonstring) {
		client.write(JWM_JSON_EXECUTE_RETURN, browserHWnd, null, jsonstring);
	}

	/**
	* ִ��js�����ȴ����ݵķ��ء�<br/>
	* �Ƽ�ʹ��executeJSON_Wait�������Լ����ַ���ת��������
	*
	* @param browserHWnd
	*            ���ĸ������ִ��
	* @param script
	*            ��ִ�е�JS�ű�
	* @param jsReturn
	*            JSִ�н����������
	* @param uuid
	*            ִ��jsʱ���ڻ�ȡ���ؽ�������jsReturn�ǹ��õģ���ô���Ը��ݴ�uuid�����ֲ�ͬ�ĵ���
	*/
	void executeJS_Wait(long browserHWnd, String script, JWebTopJSReturn jsReturn, String uuid) {
		String taskId = (uuid == null || uuid.trim().length() == 0) ? createTaskId() : uuid;
		jsReturnListners.put(taskId, jsReturn);
		client.write(JWM_JS_EXECUTE_WAIT, browserHWnd, taskId, script);
	}

	/**
	* ִ��js�������ȴ����ݵķ��ء� <br/>
	* �Ƽ�ʹ��executeJSON_NoWait�������Լ����ַ���ת��������
	*
	* @param script
	* @return
	*/
	void executeJS_NoWait(long browserHWnd, String script) {
		client.write(JWM_JS_EXECUTE_RETURN, browserHWnd, null, script);
	}
	wstring getServerName(){ return this->serverName; };
	void setServerName(wstring serverName) { this->serverName = serverName; };
	int getBlockSize() { return blockSize; };
	void setBlockSize(int blockSize) { this->blockSize = blockSize; };
	onDispatcher* getJsonHandler(){ return this->jsonHandler; };
	void setJsonHandler(onDispatcher* jsonHandler) { this->jsonHandler = jsonHandler; };


}

#endif