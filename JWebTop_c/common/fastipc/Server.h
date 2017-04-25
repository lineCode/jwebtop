//////////////////////////////////////////////////////////////////////////  
/// ����fastipc�ķ�����
/// �������ֻ�������Ŀͻ��˵�д��Ϣ��
/// �����Ҫ˫��ͨ�ţ���ô�����(S1)��Ӧ�Ŀͻ���(C1)��Ҫ����һ������ˣ�S2����Ȼ������(S1)����һ���ͻ���(C2)����д��
/// ��S1����C1��д�¼���S2����C2��д�¼���
/// ������÷����������£�create��serListerner��startRead��close��
///  
/// Դ�룺http://git.oschina.net/washheart/fastipc
/// Դ�룺https://github.com/washheart/fastipc
/// ˵����https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#ifndef FastIPC_Server_H
#define FastIPC_Server_H

#include <Windows.h>
#include <map>
#include "FastIPC.h"

namespace fastipc{
	// ����������ԭʼ����Ϣ
	class _declspec(dllexport) ReadListener {
	public:
		// ���з���˶�ȡ�����ݺ󣬻���ô˷���֪ͨ�ȴ��߽��д���
		// memBlock�ڷַ�����ɷ��������٣��ⲿ�����������������
		virtual void onRead(MemBlock* memBlock) {}
	};

	class RebuildedBlockListener_impl;// �������Ϊ��ת��Ŀ���Ǳ��⵼��STL�࣬ͬʱ���������ݷ�װ��ʵ��
	// ʵ��onRead�ԡ�ԭʼ����Ϣ������װ���̳д����ֻ��Ҫʵ��onRebuildedRead�Ϳ�����
	class _declspec(dllexport) RebuildedBlockListener :public ReadListener {
	private:
		RebuildedBlockListener_impl * impl;
		// ���з���˶�ȡ�����ݺ󣬻���ô˷���֪ͨ�ȴ��߽��д���
		// memBlock�ڷַ�����ɷ��������٣��ⲿ�����������������
		void onRead(MemBlock* readed);
	public:
		RebuildedBlockListener();
		virtual void onRebuildedRead(MemBlock* memBlock) {};
	};

	class  _declspec(dllexport) Server{
	public:
		Server();
		~Server();

	private:
		HANDLE			evtWrited;	// ����һ���¼���д��������ɣ����Զ���
		HANDLE			evtReaded;	// ����һ���¼�������������ɣ�����д��
		HANDLE			mapFile;	// �ڴ�ӳ���ļ����
		MemBuff*		memBuf;		// �������ڴ�����ݽṹ
		//DWORD			blockSize;	// ����memBuf.data�Ŀռ䳤��
		ReadListener*	listener;	// �������¼��������д���
	public:
		/// ����������
		/// @param serverName ����������
		/// @param blockSize ����memBuf.data�Ŀռ䳤�ȣ�Ĭ����2048����1024������
		/// @return 0=�ɹ�������ֵ��ʾʧ�ܣ����峣���μ�FastIPC.h�е�ERR_*����
		int	create(const std::wstring serverName, DWORD blockSize);

		/// �رշ�����
		void close(void);

		/// Ϊ����������һ�����������Ա��ȡ���ݲ����д���
		/// @param listener һ��������
		void setListener(ReadListener* listener);

		/// ��ʼ�������ݳɹ���ȡ�󣬻�ص�listener.onRead��
		/// ע��˷�����������ǰ�̡߳�
		void startRead();

		/// ������״̬�Ƿ���׼����
		/// @return true=׼����
		bool isStable();
	};
}
#endif