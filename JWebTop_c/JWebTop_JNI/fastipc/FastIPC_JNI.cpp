/// JNIʵ����
/// 
/// Դ�룺http://git.oschina.net/washheart/fastipc
/// Դ�룺https://github.com/washheart/fastipc
/// ˵����https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-31  
////////////////////////////////////////////////////////////////////////// 
#include "org_fastipc_FastIPCNative.h"
#include "jni_jdk1_6_24/jni.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "javautils.h"

JavaVM* g_jvm_server;               // ����ȫ�ֵ����������
class IPCReadListener :public fastipc::ReadListener{// ���ﲻ����RebuildedBlockListener�����ݵ���װ����java������ȥ��
private:
	jobject jlistener;
	jmethodID callBack;
public:
	int setListener(JNIEnv * env, jobject listener){
		jclass cls = env->GetObjectClass(listener);// ���Java��  
		this->callBack = env->GetMethodID(cls, "OnRead",
			//	LONG			msgType;
			//	char			packId[PACK_ID_LEN];
			//	DWORD			dataLen;
			//	char*			data = NULL;
			//  ��Ӧ��ǩ���ǣ�int,String,String
			"(ILjava/lang/String;[B)V");//��øûص��������
		if (this->callBack == NULL)return -1;
		this->jlistener = listener;
		return 0;
	}
	// ���з���˶�ȡ�����ݺ󣬻���ô˷���֪ͨ�ȴ��߽��д���
	// memBlock�ڷַ�����ɷ��������٣��ⲿ�����������������
	 void onRead(fastipc::MemBlock* memBlock) override{
		 JNIEnv *env;
		 bool detachEnv = false;
		 if (g_jvm_server->GetEnv((void **)&env, JNI_VERSION_1_6) < 0){
			 if (g_jvm_server->AttachCurrentThread((void **)&env, NULL) < 0){
				 return ;// Ӧ���ǳ����ĳ���ط�������
			 }
			 else{
				 detachEnv = true;// �����Attach�ϵ�env����ô��Ҫdetach
			 }
		 }
		 jstring packId = NULL;
		 if (memBlock->msgType != MSG_TYPE_NORMAL){
			 string pid=memBlock->getPackId();
			 packId = env->NewStringUTF(pid.c_str());
		 }

		 int len = memBlock->dataLen;
		 jbyteArray bytes = NULL;  //����һϵ�в�����btPathת��jbyteArray ���س�ȥ
		 bytes = env->NewByteArray(len);
		 env->SetByteArrayRegion(bytes, 0, len, (jbyte *)memBlock->data);
		 env->CallVoidMethod(this->jlistener, this->callBack,
			 memBlock->msgType, packId, bytes); //�ص��÷��������Ҵ��ݲ���ֵ		 
		 if (detachEnv)g_jvm_server->DetachCurrentThread();

		 // ͨ������ʽʵ�ֶ�̬�������ŵ㣺�ܶ�̬��ȱ�㣺�������ݶ���Ҫͨ�����䷽ʽ�����ܲ���
		 //this->callBack = env->GetMethodID(cls, "OnRead", "(Lorg/fastipc/FastIPCData;)V");//��øûص��������
		 //if (this->callBack == NULL)return -1;
		 //this->dataClass = env->FindClass("org/fastipc/FastIPCData"); // �õ����ڷ��������õ���  
		 //if (this->dataClass == NULL)return -2;
		 ////��õø����͵Ĺ��캯��  ������Ϊ <init> �������ͱ���Ϊ void �� V  
		 //this->dataConstroc = env->GetMethodID(this->dataClass, "<init>", "()V");
		 // �½�����
		 // ��������ֵ

		 // ͨ�����鷽ʽʵ�ֶ�̬�������ŵ㣺�ܶ�̬��ȱ�㣺intҲ��Ҫnew Object��ʽ�����ܲ���
		 //this->callBack = env->GetMethodID(cls, "OnRead", "([Ljava/lang/Object;)V");//��øûص��������
		 //jclass objClass = env->FindClass("java/lang/Object");
		 //jobjectArray rtn = env->NewObjectArray(4, objClass, NULL);
		 //jclass cls = env->FindClass("java/lang/Integer");
		 //jmethodID valueOf = env->GetStaticMethodID(cls, "valueOf", "(I)Ljava/lang/Integer;");
		 //jobject oInteger = env->CallStaticObjectMethod(cls, valueOf, 123);
		 //env->SetObjectArrayElement(rtn, 0, oInteger);
		 //jstring oString = env->NewStringUTF("abc");
		 //env->SetObjectArrayElement(rtn, 1, oString);
	 }
};
/*
* ��Ӧorg.fastipc.FastIPCNative���nCreateServer����
* �÷������ڴ���һ��fastipc�����
* serverName	���������
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* return		����һ��int[2]���顣0=�Ƿ񴴽��ɹ���1=������Server��ָ��
*/
JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateServer
(JNIEnv * env, jclass, jstring serverName, jint blockSize){
	if (g_jvm_server == NULL)env->GetJavaVM(&g_jvm_server);// ��ȡ��ǰ���������������������	
	fastipc::Server * server = new fastipc::Server();
	wstring _serverName = jstring2wstring(env, serverName);
	int result=server->create(_serverName, blockSize);
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[2];
	tmp[0] = result;
	tmp[1] = (int)server;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	delete[] tmp;
	return rtn;
}

/*
* ��Ӧorg.fastipc.FastIPCNative���nStartRead����
* �÷������ڿ���fastipc����˵Ķ�ȡ�������ɹ���������ڴ˴�
* nativeServer	ָ������ʵ����ָ��
* listener		��Ӧ��ȡ������ָ��
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nStartRead
(JNIEnv * env, jclass, jint nativeServer, jobject listener){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	IPCReadListener * ipcListener = new IPCReadListener();
	if (ipcListener->setListener(env, listener) != 0)return;
	server->setListener(ipcListener);
	server->startRead();
}

/*
* ��Ӧorg.fastipc.FastIPCNative���nCloseServer����
* �÷������ڹر�fastipc����ˣ�ִ�к�nStartRead��������ֹͣ
* nativeServer	ָ������ʵ����ָ��
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseServer
(JNIEnv *, jclass, jint nativeServer){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	server->close();
}

/*
* ��Ӧorg.fastipc.FastIPCNative���nCreateClient����
* �÷������ڴ���һ��fastipc�ͻ���
* serverName	�ͻ��˶�Ӧ�ķ���˵�����
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* return		����һ��int[2]���顣0=�Ƿ񴴽��ɹ���1=������Client��ָ��
*/
JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateClient
(JNIEnv * env, jclass, jstring serverName, jint blockSize){
	fastipc::Client * client = new fastipc::Client();
	wstring _serverName = jstring2wstring(env, serverName);
	int result = client->create(_serverName, blockSize);
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[2];
	tmp[0] = result;
	tmp[1] = (int)client;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	delete[] tmp;
	return rtn;
}
/*
* ��Ӧorg.fastipc.FastIPCNative���nWriteClient����
* �÷���������fastipc�ͻ���д�����ݣ�����д��ɺ󣬻ᴥ������˵�����
* nativeClient	ָ��ͻ���ʵ����ָ��
* data			Ҫд�������
* return		�ɹ�д������ݳ���
*/
JNIEXPORT jint JNICALL Java_org_fastipc_FastIPCNative_nWriteClient
(JNIEnv * env, jclass, jint nativeClient, jint userMsgType, jlong userValue, jstring userShortStr, jstring data){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	// data
	const char * tmp = env->GetStringUTFChars(data, false);
	int len = lstrlenA(tmp);
	char * str = new char[len];
	memcpy(str, tmp, len);
	env->ReleaseStringUTFChars(data, tmp);
	// userShortStr
	const char * tmp2 = env->GetStringUTFChars(userShortStr, false);
	int len2 = lstrlenA(tmp);
	char * shortStr = new char[len2];
	memcpy(shortStr, tmp2, len);
	env->ReleaseStringUTFChars(userShortStr, tmp2);
	//
	client->write(userMsgType,userValue,shortStr,str,len);
	delete[] str; 
	delete[] userShortStr;
	return 0;
}
/*
* ��Ӧorg.fastipc.FastIPCNative���nCloseClient����
* �÷������ڹر�fastipc�ͻ���
* nativeClient	ָ��ͻ���ʵ����ָ��
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseClient
(JNIEnv *, jclass, jint nativeClient){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	client->close();
}