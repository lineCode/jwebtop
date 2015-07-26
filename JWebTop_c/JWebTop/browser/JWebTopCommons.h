#ifndef CEF_JWEBTOP_COMMONS_H_
#define CEF_JWEBTOP_COMMONS_H_
#include <set>
#include "include\cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"
// һЩ�������ص�ͨ�úͱ�������

// �½�һ�����������
void createNewBrowser(LPCTSTR appDefFile);

// �½�һ�����������
void createNewBrowser(JWebTopConfigs configs);

namespace jc/*jc=JWebTop Client*/{
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	void CreateMessageHandlers(MessageHandlerSet& handlers);
}
#endif