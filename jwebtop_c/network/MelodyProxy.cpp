#include "MelodyProxy.h"
#include "gzip.h"
int MelodyProxy::linkCount = 0;
map<wstring, wstring> MelodyProxy::proxyMap;

// ��ȡԶ���������ݣ����������ؿͻ���
unsigned int MelodyProxy::ProxyToServer(LPVOID pParam)
{
	ProxyParam * pPar=(ProxyParam*) pParam;
	char Buffer[MAXBUFFERSIZE];
	MelodyProxy* pProxy=(MelodyProxy*)pPar->pProxy;
	char * server_name;
	unsigned int addr;
	int socket_type ;
	unsigned short port ;
	int retval,Len;
	int count=0;
	struct sockaddr_in server;
	struct hostent *hp;
	SOCKET conn_socket;
	//CString strLog;

	server_name = pPar->Address;
	socket_type = SOCK_STREAM;
	port = pPar->Port;

	if (isalpha(server_name[0])) 
	{
		/* server address is a name */
		hp = gethostbyname(server_name);
		//Log("GetHostByName");
	}
	else  
	{
		/* Convert nnn.nnn address to a usable one */
		addr = inet_addr(server_name);
		hp = gethostbyaddr((char *)&addr,4,AF_INET);
		//Log("GetHostByName");
	}

	if (hp == NULL ) 
	{
		//strLog.Format("Client: Cannot resolve address [%s]: Error %d\n",server_name,WSAGetLastError());
		//Log(strLog);

		::SetEvent(pPar->IsConnectedOK);	
		return 0;
	}
	// Copy the resolved information into the sockaddr_in structure
	//

	memset(&server,0,sizeof(server));
	/*for(int i=0;;i++){
		if(hp->h_addr_list[i]!='\0'){
			memmove(&(server.sin_addr),hp->h_addr_list[i],4);
			char *ip;
			ip=inet_ntoa(server.sin_addr);
			MessageBoxA(NULL,ip,ip,0);
		}
    }*/ 
	server.sin_family = hp->h_addrtype;
	server.sin_port = htons(port);
	memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
	conn_socket = socket(AF_INET,socket_type,IPPROTO_TCP);/* ��һ�� socket */

	if (conn_socket < 0 ) 
	{
		fprintf(stderr,"Client: Error Opening socket: Error %d\n",WSAGetLastError());
		//strLog.Format("Client: Error Opening socket: Error %d\n",WSAGetLastError());
		//Log(strLog);

		pPar->pPair->IsProxyToServClosed=TRUE;
		::SetEvent(pPar->IsConnectedOK);	
		return -1;
	}

	//Log("���ӷ�����. . .");
	if (connect(conn_socket,(struct sockaddr*)&server,sizeof(server))== SOCKET_ERROR) 
	{
		//
		//strLog.Format("connect() failed: %d\n",WSAGetLastError());
		pPar->pPair->IsProxyToServClosed=TRUE;	
		::SetEvent(pPar->IsConnectedOK);	
		return -1;
	}
	//Log("���ӷ������ɹ�");
	if(port==443){
		char* buffer="HTTP/1.1 200 Connection established\r\n\r\n";
		retval = send(pPar->pPair->ProxyToUserSocket,buffer,strlen(buffer),0);
		if (retval == 0) 
		{
			fprintf(stderr,"recv() failed: error %d\n",WSAGetLastError());
		}
	}

	pPar->pPair->ProxyToServSocket = conn_socket;
	pPar->pPair->IsProxyToServClosed = FALSE;
	::SetEvent(pPar->IsConnectedOK);

	int length=-1;
	char header[MAXBUFFERSIZE]={0};
	char* content = new char[MAXBUFFERSIZE*50];
	ZeroMemory(content,MAXBUFFERSIZE*50);
	char* endContentP=content;
	bool isChunked=false;
	bool isContentLength=false;
	int resChunkCount=0;
	bool isFirst=true;
	pPar->Response=NULL;
	while(!pPar->pPair->IsProxyToServClosed && !pPar->pPair->IsProxyToUserClosed)
	{
		//Log("׼������Ŀ���ַ������");
		retval = recv(conn_socket,Buffer,sizeof (Buffer),0 );
		//Log("����Ŀ���ַ������");

		if (retval == SOCKET_ERROR ) 
		{
			fprintf(stderr,"recv() failed: error %d\n",WSAGetLastError());
			//pProxy->winHandler->agentRequest(pPar->Request);

			//Log("���ӷ�����. . .");
			closesocket(conn_socket);
			if(!pPar->pPair->IsProxyToServClosed){
				pPar->pPair->IsProxyToServClosed=TRUE;
			}
			break;
		}

		if (retval == 0) 
		{
			//Log("Server closed connection\n");
			closesocket(conn_socket);
			pPar->pPair->IsProxyToServClosed=TRUE;
			break;
		}
		Len=retval;
		if(Len<MAXBUFFERSIZE){
			Buffer[Len]=0;
		}
		//��һ���յ�������ͷ��
		if(isFirst&&!pPar->isHttps){
			//���ж��Ƿ���Transfer-Encoding: chunked
			char* start=strstr(Buffer,"\r\n\r\n");
			while(start){
				char* p = strstr(Buffer,"Content-Type: ");
				char contentType[100]={0};
				if(p){
					p+=strlen("Content-Type: ");
					char* end=strstr(p,"\r\n");
					if(end){
						strncpy(contentType,p,end-p);
					}
				}
				if(!strstr(contentType,"text")&&!strstr(contentType,"javascript")){
					break;
				}
				//�ָ�ͷ��
				int headerLength=start-Buffer+4;//ͷ������
				strncpy(header,Buffer,headerLength);
				header[headerLength]=0;
				p = strstr(Buffer,"Transfer-Encoding: chunked");
				//chunked����
				if(p){
					if(Len-headerLength>0){
						resChunkCount=readChunk(start+4,Len-headerLength,&endContentP,pProxy);
					}
					isChunked=true;
				}
				//�ж��Ƿ���Content-Length
				else if(p=strstr(Buffer,"Content-Length: ")){
					char temp1[100];
					p+=strlen("Content-Length: ");
					int l=strlen(p);
					if(l>30)l=30;
					strncpy(temp1,p,l);
					for(char* i=temp1;i<temp1+30;++i){
						if(i[0]=='\r'){
							i[0]=0;
							break;
						}
					}
					length=atoi(temp1);
					isContentLength=true;
					count=Len-headerLength;
					if(count>0){
						memcpy((void *)content,start+4,count);
						content[count]=0;
					}
				}
				isFirst=false;
				break;
			}
			if(!start){
				//MessageBoxA(NULL,Buffer,Buffer,0);
			}
		}
		else if(isChunked){
			if(resChunkCount>0){
				if(Len>resChunkCount){
					memcpy((void *)endContentP,Buffer,resChunkCount);
					endContentP+=resChunkCount;
					resChunkCount=readChunk(Buffer+resChunkCount+2,Len-resChunkCount-2,&endContentP,pProxy);
				}
				else{
					memcpy((void *)endContentP,Buffer,Len);
					endContentP+=Len;
					resChunkCount-=Len;
				}
			}
			else{
				resChunkCount=readChunk(Buffer,Len,&endContentP,pProxy);
			}
		}
		else if(isContentLength){
			memcpy(content+count,Buffer,Len);
			count+=Len;
		}
		if(isChunked){
			if(resChunkCount==-1){
				isChunked=false;
				int l=endContentP-content;
				char buf[100];
				sprintf(buf,"%s%d","Content-Length: ",l);
				char header2[MAXBUFFERSIZE];
				replace(header,"Transfer-Encoding: chunked",buf, header2,MAXBUFFERSIZE);
				retval=Response(header2, content, l, pProxy, pPar);
				if(retval == SOCKET_ERROR){
					break;
				}
				isFirst=true;
				isChunked=false;
				resChunkCount=0;
				isContentLength=false;
				length=-1;
				endContentP=content;
			}
		}
		else if(isContentLength){
			if(count>=length){
				retval=Response(header, content, count, pProxy, pPar);
				if(retval == SOCKET_ERROR){
					break;
				}
				int res=count-length;
				if(res>0){
					MessageBoxA(NULL,Buffer+Len-res,Buffer+Len-res,0);
				}
				count=0;
				isFirst=true;
				isContentLength=false;
				length=-1;
				isChunked=false;
				resChunkCount=0;
				endContentP=content;
			}
		}
		else{
			retval = send(pPar->pPair->ProxyToUserSocket,Buffer,Len,0);
			//Sleep(100);
			//pProxy->winHandler->agentResponse(Buffer,Buffer,Buffer,pPar);
			if (retval == SOCKET_ERROR) 
			{
				fprintf(stderr,"send() failed: error %d\n",WSAGetLastError());
				closesocket(pPar->pPair->ProxyToUserSocket);
				pPar->pPair->IsProxyToUserClosed=TRUE;
				break;						
			}
		}

		//Log("���û���ַ��������");
	}
	if(pProxy->agentRequest){
		//pProxy->winHandler->agentResponse(response);
	}
	if(pPar->pPair->IsProxyToServClosed==FALSE)
	{
		closesocket(pPar->pPair->ProxyToServSocket);
		pPar->pPair->IsProxyToServClosed=TRUE;
	}
	if(pPar->pPair->IsProxyToUserClosed==FALSE)
	{
		closesocket(pPar->pPair->ProxyToUserSocket);
		pPar->pPair->IsProxyToUserClosed=TRUE;
	}
	if(pPar->Response!=NULL){
		delete pPar->Response;
		pPar->Response=NULL;
	}
	::CloseHandle(pPar->ReplaceResponseOK);
	delete content;
	return 1;
}
int MelodyProxy::Response(char* header, char* content, int count, MelodyProxy* pProxy, ProxyParam* pPar){
	char* bufferAll=new char[MAXBUFFERSIZE*50];
	int retval=0;
	//�����gzip���Ƚ���
	char *p=strstr(header,"Content-Encoding: gzip");
	uLong contentLength=count;
	if(p){
		contentLength=MAXBUFFERSIZE*50;
		Byte* data=new Byte[MAXBUFFERSIZE*50];
		httpgzdecompress((Byte*)content, count, data, &contentLength);
		char buf[100],buf1[100];
		sprintf(buf,"%s%ld","Content-Length: ",contentLength);
		sprintf(buf1,"%s%d","Content-Length: ",count);
		char header3[MAXBUFFERSIZE]={0},header4[MAXBUFFERSIZE]={0};
		replace(header,buf1,buf, header3, MAXBUFFERSIZE);
		replace(header3,"Content-Encoding: gzip\r\n","",header4,MAXBUFFERSIZE);
		header=header4;
		memcpy(content,data,contentLength);
		content[contentLength]=0;
		delete data;
	}
	strcat(bufferAll,header);
	memcpy(bufferAll+strlen(header),content,contentLength);
	MelodyProxy *proxy=(MelodyProxy *)pPar->pProxy;
	pProxy->winHandler->agentResponse(pPar->Request,header,content,pPar);
	if(pProxy->replaceResponse){
		::WaitForSingleObject(pPar->ReplaceResponseOK,INFINITE);
		if(pPar->CancelReplaceResponse){
			retval = send(pPar->pPair->ProxyToUserSocket,bufferAll,strlen(header)+contentLength,0);
		}
		else{
			retval = send(pPar->pPair->ProxyToUserSocket,pPar->Response,strlen(pPar->Response),0);
		}
	}
	else{
		retval = send(pPar->pPair->ProxyToUserSocket,bufferAll,strlen(header)+contentLength,0);
	}
	if (retval == SOCKET_ERROR) 
	{
		fprintf(stderr,"send() failed: error %d\n",WSAGetLastError());
		closesocket(pPar->pPair->ProxyToUserSocket);
		pPar->pPair->ProxyToUserSocket=TRUE;
	}
	delete []bufferAll;
	return retval;
}