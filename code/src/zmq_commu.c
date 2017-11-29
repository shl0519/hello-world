
#include "include.h"

pthread_t threID_proxy = -1;
pthread_t threID_getfilesvr = -1;
pthread_t threID_getmsgsvr = -1;

int fmt2str_msg(char *inbuf,int inlen,char *outbuf)
{
	if((inbuf==NULL)||(inlen==0)||(outbuf == NULL))
	{
		writelog(ZLOG_ERR, "[strfmt_msg] Invalid argument!");
		return -1;
	}
	int i=0;	
	char tmpbuf[8];
	for(i=0; i<inlen;i++)
	{
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"%02X",inbuf[i]);
		strcat(outbuf,tmpbuf);
	}
	return 0;
}

/*******************************************************************************
* ������  : zmq_send_msq
* ����	  : 3352������������Ϣ�����ַ�����ʽ�����ڶ�����
* ����	  : char *ip,�����IP��ַ
		  : int port,����˵ķ���˿�
		  : char *msg,���͵���Ϣ
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int zmqsend_msg(char *destip,int destport,char *msg,int len)
{
	if((destip==NULL)||(destport==0)||(msg == NULL)||(len == 0))
	{
		writelog(ZLOG_ERR, "[zmq_c2s_msq] Invalid argument!");
		return -1;
	}
	void * pCtx = NULL;
	void * pSock = NULL;
	char pAddr[32];
	memset(pAddr,0,sizeof(pAddr));
	sprintf(pAddr,"tcp://%s:%d",destip,destport);
	
	if((pCtx = zmq_ctx_new()) == NULL)
	{
		return -1;
	}
	if((pSock = zmq_socket(pCtx, ZMQ_REQ)) == NULL)
	{
		zmq_ctx_destroy(pCtx);
		return -1;
	}
	int timeout=0;
	timeout = ZMQ_RECV_OVT;
	if(zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &timeout, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	} 
	
	timeout= ZMQ_SEND_OVT;
	if(zmq_setsockopt(pSock, ZMQ_SNDTIMEO, &timeout, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	int linger = 0;
	if(zmq_setsockopt(pSock, ZMQ_LINGER, &linger, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	if(zmq_connect(pSock, pAddr) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}
	
	char szMsg[ZMQ_MAX_MSG_LEN];
	char buf[ZMQ_MAX_MSG_LEN];
	memset(szMsg,0,ZMQ_MAX_MSG_LEN);
	memset(buf,0,ZMQ_MAX_MSG_LEN);
	
	fmt2str_msg(msg,len,buf);
	
	int ilen;
	ilen = zmq_send(pSock, msg, len, 0);
	if(ilen != len)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		writelog(ZLOG_MNG, "[zmqsend_msg] send msg : %s faile", buf);
		writelog(ZLOG_MNG, "[zmqsend_msg] send message faile,errorno=:%s",zmq_strerror(errno));
		return -1;
	}
	
	writelog(ZLOG_MNG, "[zmqsend_msg] send msg : %s success", buf);
	memset(buf,0,sizeof(buf));
	ilen=0;
	ilen = zmq_recv(pSock, szMsg, ZMQ_MAX_MSG_LEN, 0);
	fmt2str_msg(szMsg,ilen,buf);
	if(ilen < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		writelog(ZLOG_MNG, "[zmqsend_msg] send msg get recv : %s faile", buf);
		writelog(ZLOG_MNG, "[zmqsend_msg] zmq_recv errorno=:%s",zmq_strerror(errno));
		return -1;
	}
	
	zmq_close(pSock);
	zmq_ctx_destroy(pCtx);
	writelog(ZLOG_MNG, "[zmqsend_msg] received message:%s  success",buf);
	return 0;
}


/*******************************************************************************
* ������  : zmq_s2c_msq
* ����	  : �������3352������Ϣ
* ����	  : int svrport,���ط���˿�
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int zmqget_msg(int *svrport)
{
	if(svrport==NULL)
	{
		writelog(ZLOG_ERR, "[zmqget_msg] Invalid argument!");
		return -1;
	}
	void * pCtx = NULL;
	void * pSock = NULL;
	char pAddr[16];
	memset(pAddr,0,sizeof(pAddr));
	sprintf(pAddr,"tcp://*:%d",*svrport);

	if((pCtx = zmq_ctx_new()) == NULL)
	{
		return -1;
	}

	if((pSock = zmq_socket(pCtx, ZMQ_REP)) == NULL)
	{
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	int timeout=0;
	
#if 0  //��������

	timeout = ZMQ_RECV_OVT;
	if(zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &timeout, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	} 
#endif
	
	timeout= ZMQ_SEND_OVT;
	if(zmq_setsockopt(pSock, ZMQ_SNDTIMEO, &timeout, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	if(zmq_bind(pSock, pAddr) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}
	
	int len;
	int anlysres;
	int cmd;
	char szMsg[ZMQ_MAX_MSG_LEN];
	char buf[ZMQ_MAX_MSG_LEN];
	char zbuf[ZMQ_MAX_MSG_LEN];

	for(;;)
	{
		memset(szMsg,0,ZMQ_MAX_MSG_LEN);
		memset(buf,0,ZMQ_MAX_MSG_LEN);
		len = zmq_recv(pSock, szMsg, ZMQ_MAX_MSG_LEN, 0);
		if(len < 0)
		{
			writelog(ZLOG_MNG, "[zmqget_msg] received message:%s error",zmq_strerror(errno));
			continue;
		}

		fmt2str_msg(szMsg,len,buf);
		writelog(ZLOG_MNG, "[zmqget_msg] received message : %s", buf);

		/* �յ���Ϣ���������������������� */
		memset(buf,0,ZMQ_MAX_MSG_LEN);
		anlysres=0;
		cmd=0;
		if(m2lmsg_anlys(szMsg,len,buf,&anlysres,&cmd) == 0)
		{
			len=0;
			memset(szMsg,0,ZMQ_MAX_MSG_LEN);
			exec_m2lmsg(buf, cmd, szMsg);
			memset(buf,0,ZMQ_MAX_MSG_LEN);
			fmtrly_m2lmsg(szMsg,cmd,EXEC_SUCCESS,buf,&len);
		}
		else
		{
			len=0;
			memset(buf,0,ZMQ_MAX_MSG_LEN);
			fmtrly_m2lmsg(NULL,cmd,anlysres,buf,&len);
		}
		
		memset(szMsg,0,ZMQ_MAX_MSG_LEN);
		fmt2str_msg(buf,len,szMsg);

		if(zmq_send(pSock, buf, len, 0) < 0)
		{
			writelog(ZLOG_MNG, "[zmqget_msg] send reply:%s error",zmq_strerror(errno));
			writelog(ZLOG_MNG, "[zmqget_msg] send reply : %s faile", szMsg);
			continue;
		}
		writelog(ZLOG_MNG, "[zmqget_msg] send reply : %s success", szMsg);
		if(cmd == REBOOT)
		{
			writelog(ZLOG_MNG,"�ظ���ɣ������豸");
			sleep(1);
			reboot(RB_AUTOBOOT);//����ָ������ִ�У��Ȼظ�ִ�гɹ���֮����ִ��
			break;
		}
	}
	
	zmq_close(pSock);
	zmq_ctx_destroy(pCtx);
	return 0;
}


/*******************************************************************************
* ������  : zmq_c2s_file
* ����	  : 3352�����������ļ�
* ����	  : char *ip,Ŀ��IP��ַ
		  : int port,Ŀ�����˿�
		  : char *name,�ļ���
		  : char *path,�ļ�����Ŀ¼
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int zmqsend_file(char *destip,int destport,char *srcname,char *srcpath)
{
	if((destip==NULL)||(destport==0)||(srcname == NULL)||(srcpath == NULL))
	{
		writelog(ZLOG_ERR, "[zmqsend_file] Invalid argument!");
		return -1;
	}
	void * pCtx = NULL;
	void * pSock = NULL;
	char pAddr[32];
	memset(pAddr,0,sizeof(pAddr));
	sprintf(pAddr,"tcp://%s:%d", destip, destport);

	if((pCtx = zmq_ctx_new()) == NULL)
	{
		return 0;
	}
	if((pSock = zmq_socket(pCtx, ZMQ_REQ)) == NULL)
	{
		zmq_ctx_destroy(pCtx);
		return 0;
	}
	
	int timeout=0;
	timeout = ZMQ_RECV_OVT;
	if(zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &timeout, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	} 
	
	timeout= ZMQ_SEND_OVT;
	if(zmq_setsockopt(pSock, ZMQ_SNDTIMEO, &timeout, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	int linger = 0;
	if(zmq_setsockopt(pSock, ZMQ_LINGER, &linger, sizeof(int)) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	if(zmq_connect(pSock, pAddr) < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	char szMsg[512] = {0};
	FILE *fp;
	strcpy(szMsg,srcpath);
	strcat(szMsg,srcname);
	if( (fp = fopen(szMsg, "rb")) == NULL )
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		writelog(ZLOG_ERR, "[zmqsend_file] fopen : %s faile", szMsg);
		return -1;
	}
	
	memset(szMsg,0,sizeof(szMsg));
	int len=0;
	int filesize;
	int sendsize;
	len = s_sendmore(pSock, srcname, strlen(srcname));
	if(len != strlen(srcname))
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		writelog(ZLOG_MNG, "[zmqsend_file] commu file name error");
		return -1;
	}
	len=0;
	filesize=0;
	sendsize=0;
	while((len = fread(szMsg, 1, 100, fp)) > 0 )/* zmqһ����ഫ��256���ֽڣ��ɽ��ն˵�buf���� */
	{
		filesize=filesize + len;
		sendsize = sendsize + s_sendmore(pSock, szMsg, len);
		memset(szMsg, 0, 100);
	}
	fclose(fp);
	s_send(pSock, "OK", 2);
	writelog(ZLOG_MNG, "[zmqsend_file] filesize = %d,sendsize=%d",filesize,sendsize);
	
	char *rly = NULL;
	rly = s_recv(pSock);
	writelog(ZLOG_MNG, "[zmqsend_file] recv info:%s",rly);
	free(rly);
	writelog(ZLOG_MNG, "[zmqsend_file] file send over");
	return 0;
}


/*******************************************************************************
* ������  : thre_get_file
* ����	  : �����ļ��̺߳����壬��ͨ�̣߳����Ȳ��Ժ����ȼ��ȵȶ�û������
* ����	  : *pCtx��������zmq����
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int thre_get_file(void *pCtx)
{
	void *pSock = NULL;
	if((pSock = zmq_socket(pCtx, ZMQ_REP))==NULL)
	{
		zmq_ctx_destroy(pCtx);
		return -1;
	}
	if(zmq_connect(pSock, "inproc://workers") < 0)
	{
		zmq_close(pSock);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	for(;;)
    {
		FILE *fp = NULL;
		char *info = NULL;
		char *file = NULL;

		/* �ж��Ƿ���յ��ļ�����Ϣ*/
		file = s_recv(pSock);
		if( file == NULL )
		{
			writelog(ZLOG_ERR, "[thre_get_file] file is null");
			free(file);
			continue;
		}
		
		
		char buf[128];
		char pathfile[128];
		int len=0;
		memset(buf,0,128);
		memset(pathfile,0,128);
		strcpy(pathfile,STR_RECV_FILE_PATH);/* �洢·�� */
		strcat(pathfile,file);
		
		
		fp = fopen(pathfile, "wb");
		if( fp == NULL )
		{
			writelog(ZLOG_ERR, "[thre_get_file] open file faile");
			free (file);
			continue;
		}
		strcpy(buf,file);
		for(;;)
		{
			info = s_frecv(pSock, fp);
			if( strcmp(info, "OK") == 0 )
			{
				break;
			}
			else
			{
				len = len + strlen(info);
				free(info);
				info = NULL;
			}
		}
		free(file);
		free(info);
		fclose(fp);
		char tmp[128];
		sprintf(tmp,"receive %s %d bytes success",buf,len);
		s_send(pSock, tmp, strlen(tmp));
		writelog(ZLOG_MNG, "receive %s %d bytes success",buf,len);

		/* �յ���Ϣ���������������������� */
		int i;
		for(i=0; i<TAB_TOTAL;i++)
		{
			if(strcmp(buf,m2l_txtname[i]) == 0)
			{
				break;
			}
		}
		if(i >= TAB_TOTAL)
		{
			writelog(ZLOG_MNG, "�յ���Ч�ļ���ɾ��");
			delfile(pathfile);
			continue;
		}
		rdtxt_instab(i);	
	}
	return -1;
}

/*******************************************************************************
* ������  : zmqproxy
* ����	  : ���̣߳����������İ󶨣��ú���������һ�����߳�
* ����	  : svrport������˿�
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int zmqproxy(void *svrport)
{
	if(svrport==NULL)
	{
		writelog(ZLOG_ERR, "[zmqproxy] Invalid argument!");
		return -1;
	}
	void *pCtx = NULL;
	void *pClts = NULL;
	char pAddr[16];
	int *port;
	port=(int*)svrport;
	memset(pAddr,0,sizeof(pAddr));
	sprintf(pAddr,"tcp://*:%d",*port);

	if((pCtx = zmq_ctx_new()) == NULL)
	{
		return -1;
	}
	if((pClts = zmq_socket(pCtx, ZMQ_ROUTER)) == NULL)
	{
		zmq_ctx_destroy(pCtx);
		return -1;
	}
	if(zmq_bind(pClts, pAddr) < 0)
	{
		zmq_close(pClts);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	void *workers = NULL;
	if((workers = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)
	{
		zmq_close(pClts);
		zmq_ctx_destroy(pCtx);
		return -1;
	}	
	if(zmq_bind(workers, "inproc://workers") < 0)
	{
		zmq_close(workers);
		zmq_close(pClts);
		zmq_ctx_destroy(pCtx);
		return -1;
	}

	if( pthread_create(&threID_getfilesvr, NULL, (void *)thre_get_file, pCtx)!=0 )
	{
		writelog(ZLOG_ERR, "[zmqproxy]pthread_create faile");
		zmq_close(workers);
		zmq_close(pClts);
		zmq_ctx_destroy(pCtx);
		return -1;
	}
	writelog(ZLOG_SYS, "[zmqproxy]create thread thre_get_file successful");
	zmq_proxy(pClts, workers, NULL);//�����ڵ�ǰ���߳̿ռ������У�����ֻ���ڵ�ǰʹ�õ�context���ر�֮��Ż᷵��
	zmq_close(workers);
	zmq_close(pClts);
	zmq_ctx_destroy(pCtx);
	return -1;
}

/*******************************************************************************
* ������  : getfilesvr_init
* ����	  : ������ȡ�ļ����񣬸ú�������һ���߳�
* ����	  : svrport������˿�
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int getfilesvr_init(int *svrport)
{
	if( pthread_create(&threID_proxy, NULL, (void *)zmqproxy, (void*)svrport) != 0 )
	{
		writelog(ZLOG_ERR, "[getfilesvr_init]pthread_create faile");
		return -1;
	}
	writelog(ZLOG_SYS, "[getfilesvr_init]create thread zmqproxy  successful");
	return 0;
}
/*******************************************************************************
* ������  : getmsgsvr_init
* ����	  : ������ȡ��Ϣ���񣬸ú�������һ���߳�
* ����	  : svrport������˿�
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int getmsgsvr_init(int *svrport)
{
	if( pthread_create(&threID_getmsgsvr, NULL, (void *)zmqget_msg, (void*)svrport) != 0 )
	{
		writelog(ZLOG_ERR, "[getmsgsvr_init]pthread_create faile");
		return -1;
	}
	writelog(ZLOG_SYS, "[getmsgsvr_init]create get msg svr successful");
	return 0;
}



