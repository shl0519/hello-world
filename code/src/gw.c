#include "include.h"

/*----------------------------------------------------
* ������  	: sendbuf_fmt
* ����	  	: ���ͻ�������ʽ������������Ϸ����ж�
* ����    	: @cmd : ������
			: @cmdID : ����ID
			: @content : ����
			: @len : ���ݳ���
		  
* ���	  : @buf : �������
* ����	  : 0: ����
			<0:�쳣
----------------------------------------------------*/
int sendbuf_fmt(unsigned int cmd, unsigned int cmdID, 
				unsigned char *content,unsigned len,unsigned char *buf,int *outlen)
{

	if((cmd != SET_GW_INFO)&&(cmd != GET_GW_RES))
	{
		writelog(ZLOG_ERR, "�����ִ���:%08x",cmd);
		return -1;
	}
	if(buf == NULL)
	{
		writelog(ZLOG_ERR, "���ڲ�������");
		return -1;	
	}
	unsigned int calcrc;/* �����CRC��� */
	
	buf[0]=0xAA;
	buf[1]=0xAA;
	buf[2]=0xAA;
	buf[3]=0xAA;

	uint2ubuf(cmd,&buf[4]);				/* ������ */
	uint2ubuf(cmdID,&buf[8]);			/* ID */
	uint2ubuf(len,&buf[12]);			/* ���ݳ��� */
	if(content != NULL)
	{
		memcpy(&buf[16],content,len);	/* ���� */
	}


	StringCrc32(&buf[4],&calcrc,len+12);/* ����CRC32 */
	uint2ubuf(calcrc,(unsigned char *)&buf[len+16]);

	buf[len+20]=0x55;
	buf[len+21]=0x55;
	buf[len+22]=0x55;
	buf[len+23]=0x55;
	*outlen=len+24;
	return 0;
}

/*----------------------------------------------------
* ������  : content_alys
* ����	  : ���ܸ�ʽ����
* ����    : @ip : ����ת�����豸��IP
		  : @port : �˿�
		  : @money : ���׽��
		  : @paynumber :�������

* ���	  : ��
* ����	  : 0: ����
			<0:�쳣
----------------------------------------------------*/
int content_alys(unsigned char *buf,unsigned int len, unsigned int cmd,gw_node_s *n)
{
	if((buf == NULL)||(n == NULL))
	{
		printf("[n2s_recvalys]:Invalid argument!");
		return -1;		
	}
	switch(cmd)
	{
		case SET_GW_INFO:
		{
			if(len != MAX_GWSPACE_LEN)				/* ���س��ȴ��� */
			{
				return -1;
			}
			int i;			
			for(i=0; i<len; i++)
			{
				n->SpaceInfo[i].NowStat = (*(buf+i))&0x01;
				if(((*(buf+i))&0xC0) == 0x00)
				{
					n->SpaceInfo[i].DevErr = DETOR_COMMU_ERR;
					writelog(ZLOG_GW, "����:%s  ��λ%s �쳣",n->gwIP,n->SpaceInfo[i].SpaceNO);
				}
				else 
				{
					n->SpaceInfo[i].DevErr = 0;
				}
			}
			break;	

		}
		case GET_GW_RES:
		{
			if(len != MAX_GWSPACE_LEN)				/* ���س��ȴ��� */
			{
				return -1;
			}

			int i;			
			for(i=0; i<len; i++)
			{
				n->SpaceInfo[i].NowStat = (*(buf+i))&0x01;
				if(((*(buf+i))&0xC0) == 0x00)
				{
					n->SpaceInfo[i].DevErr = DETOR_COMMU_ERR;
					writelog(ZLOG_GW, "����:%s  ��λ%s �쳣",n->gwIP,n->SpaceInfo[i].SpaceNO);
				}
				else 
				{
					n->SpaceInfo[i].DevErr = 0;
				}
			}
			break;	
		}
		default:
		{
			writelog(ZLOG_ERR, "��ʶ�������:%08x",cmd);
			return -1;
			break;
		}
	}
	return 0;
}
/*----------------------------------------------------
* ������  : recv_alys
* ����	  : ���ܸ�ʽ����
* ����    : @ip : ����ת�����豸��IP
		  : @port : �˿�
		  : @money : ���׽��
		  : @paynumber :�������

* ���	  : ��
* ����	  : 0: ����
			<0:�쳣
----------------------------------------------------*/
int recv_alys(unsigned char *buf,unsigned int len, unsigned int cmd,
				unsigned int cmdID,gw_node_s *n)
{
	if((buf == NULL) || (len <= 0))
	{
		printf("[n2s_recvalys]:Invalid argument!");
		return -1;		
	}

	unsigned int j;
	unsigned int icmd;
	unsigned int icmdID;
	unsigned int execres;
	unsigned int icrc;
	unsigned int calcrc;
	unsigned int ilen;
	j=0;
	memcpy(&j,&buf[0],4);		/* ���ջ�Ӧ֡ͷ */
	if(j != 0xAAAAAAAA)
	{
		writelog(ZLOG_ERR,"���ջ�Ӧ֡ͷ����");
		return -1;
	}
	
	j=0;
	memcpy(&j,&buf[len-4],4);	/* ���ջ�Ӧ֡β */
	if(j != 0x55555555)
	{
		writelog(ZLOG_ERR,"���ջ�Ӧ֡β����");
		return -1;
	}

	icmd=0;
	ubuf2uint(&buf[4],&icmd);	/* ������ */	
	if(icmd != cmd)
	{
		writelog(ZLOG_ERR,"�����ֲ�һ��");
		return -1;
	}

	icmdID=0;
	ubuf2uint(&buf[8],&icmdID);	/* ����ID */
	if(icmdID != cmdID)
	{
		writelog(ZLOG_ERR,"����ID��һ��");
		return -1;
	}

	execres=0;
	ubuf2uint(&buf[12],&execres);	/* ִ�н�� */
	if(execres != EXEC_SUCCESS)
	{
		writelog(ZLOG_ERR,"ִ�н������");
		return -1;
	}

	icrc=0;
	ubuf2uint(&buf[len-8],&icrc);/* CRCУ�� */
	StringCrc32(&buf[4],&calcrc,len-12);
	printf("-------------:%x,,,%x\r\n",icrc,calcrc);
	if(icrc != calcrc)
	{
		writelog(ZLOG_ERR,"CRCУ�鲻һ��");
		return -1;
	}

	ilen=0;
	ubuf2uint(&buf[16],&ilen);	/* ���ݳ��� */
	j=0;
	j=content_alys(&buf[20],ilen,icmd,n);
	if(j != 0)
	{
		writelog(ZLOG_ERR,"�������ݴ������");
		return -1;
	}	

	return 0;

}

/*-----------------------------------------------------------------------
* ������  	: sendbuf_fmt
* ����	  	: ���ͻ�������ʽ������������Ϸ����ж�,�ýӿ�����һ��������
* ����    	: @cmd : ������
			: @cmdID : ����ID
			: @content : ����
			: @len : ���ݳ���
		  
* ���	  : @buf : �������
* ����	  : 0: ����
			<0:�쳣
-------------------------------------------------------------------------*/
int getinfo_gw(gw_node_s *n,unsigned int cmdID)
{
	int fd;
	int j;
	j=tcp_blkConn(n->gwIP,n->gwPort,&fd);
	//j=tcp_blkConn("192.168.6.13",9665,&fd);
	if(j != 0)
	{
		tcp_blkClose(&fd);
		writelog(ZLOG_ERR, "[getinfo_gw]%s:%d connect faile",n->gwIP, n->gwPort);
		return -1;
	}

	char buf[512];
	int outlen;
	int sendlen;
	char printbuf[1024];
	memset(buf,0,sizeof(buf));
	memset(printbuf,0,sizeof(printbuf));
	j=sendbuf_fmt(GET_GW_RES, cmdID, NULL, 0, buf, &outlen);
	if(j != 0)
	{
		tcp_blkClose(&fd);
		writelog(ZLOG_ERR, "[getinfo_gw] %s:%d send format error",n->gwIP, n->gwPort);
		return -1;
	}
	
	fmt2str_msg(buf,outlen,printbuf);

	sendlen = send(fd,buf,outlen,0);
	if( sendlen != outlen)/* ���Ͳɼ���ϸָ���ǵİ�������20 */
	{
		writelog(ZLOG_ERR,"[getinfo_gw]send to %s:%d over time,error:%s,errorno:%d",n->gwIP, n->gwPort,strerror(errno),errno);
		writelog(ZLOG_GW, "[getinfo_gw] send cmd : %s faile", printbuf);
		tcp_blkClose(&fd);
		return -1;
	}
	
	writelog(ZLOG_GW, "[getinfo_gw] send cmd : %s success", printbuf);

	memset(buf, 0, sizeof(buf));
	j = recv(fd,buf,sizeof(buf),0);
	if(j < 0)
	{
		writelog(ZLOG_ERR,"[getRes_gw] recv %s:%d  over time,error:%s,errno:%d",n->gwIP, n->gwPort,strerror(errno),errno);
		tcp_blkClose(&fd);
		return -1;
	}
	tcp_blkClose(&fd);//�����رշ���
	
	memset(printbuf,0,sizeof(printbuf));
	fmt2str_msg(buf,j,printbuf);
	writelog(ZLOG_GW, "[getinfo_gw] get reply : %s success", printbuf);

	return(recv_alys(buf,j,GET_GW_RES,cmdID,(void*)n));
}
/*-----------------------------------------------------------------------
* ������  	: sendbuf_fmt
* ����	  	: ���ͻ�������ʽ������������Ϸ����ж�,�ýӿ�����һ��������
* ����    	: @cmd : ������
			: @cmdID : ����ID
			: @content : ����
			: @len : ���ݳ���
		  
* ���	  : @buf : �������
* ����	  : 0: ����
			<0:�쳣
-------------------------------------------------------------------------*/
int setinfo_gw(gw_node_s *n,unsigned int cmdID)
{
	int fd;
	int i,j;
	j=tcp_blkConn(n->gwIP,n->gwPort,&fd);
	if(j != 0)
	{
		tcp_blkClose(&fd);
		writelog(ZLOG_ERR, "[setinfo_gw] %s:%d connect faile",n->gwIP,n->gwPort);
		return -1;
	}

	unsigned char buf[512];
	char printbuf[1024];
	int sendlen;
	int outlen;
	memset(buf,0,sizeof(buf));
	memset(printbuf,0,sizeof(printbuf));

	unsigned char setbuf[512];
	memset(setbuf,0,sizeof(setbuf));
	
	for(i=0;i<MAX_GWSPACE_LEN;i++)
	{
		if(n->SpaceInfo[i].SpaceID == 0)
		{
			setbuf[i]=0;
			continue;
		}
		setbuf[i] = 0 | (n->SpaceInfo[i].InUse << 3) | (n->SpaceInfo[i].UseLight << 2)
				  	  | (n->SpaceInfo[i].BlueLight << 1);

	}

	j=sendbuf_fmt(SET_GW_INFO, cmdID, setbuf, MAX_GWSPACE_LEN, buf,&outlen);
	if(j != 0)
	{
		tcp_blkClose(&fd);
		writelog(ZLOG_ERR, "[setinfo_gw] %s:%d send format error",n->gwIP,n->gwPort);
		return -1;
	}

	fmt2str_msg(buf,outlen,printbuf);
	sendlen = send(fd,buf,outlen,0);

	if( sendlen != outlen)/* ���Ͳɼ���ϸָ���ǵİ�������20 */
	{
		writelog(ZLOG_ERR,"[setinfo_gw]send to %s:%d over time,error:%s,errorno:%d",n->gwIP, n->gwPort,strerror(errno),errno);
		writelog(ZLOG_GW, "[setinfo_gw] send cmd : %s faile", printbuf);
		tcp_blkClose(&fd);
		return -1;
	}
	
	writelog(ZLOG_GW, "[setinfo_gw] send cmd : %s success", printbuf);


	memset(buf, 0, sizeof(buf));
	memset(printbuf,0,sizeof(printbuf));
	j = recv(fd,buf,sizeof(buf),0);
	if(j < 0)
	{
		writelog(ZLOG_ERR,"[setinfo_gw] %s:%d  TCP recv over time,error:%s,errno:%d",n->gwIP,n->gwPort,strerror(errno),errno);
		tcp_blkClose(&fd);
		return -1;
	}
	tcp_blkClose(&fd);//�����رշ���0

	fmt2str_msg(buf,j,printbuf);
	writelog(ZLOG_GW, "[setinfo_gw] get reply : %s success", printbuf);

	return(recv_alys(buf, j, SET_GW_INFO, cmdID, n));
}

/*-----------------------------------------------------------------------
* ������  	: chkset_gwlist
* ����	  	: �����������,����3�����ò��ɹ�����ôĬ�ϳɹ�
* ����    	: @cmd : ������
			: @cmdID : ����ID
			: @content : ����
			: @len : ���ݳ���
		  
* ���	  : @buf : �������
* ����	  : 0: ����
			<0:�쳣
-------------------------------------------------------------------------*/
int chkset_gwlist(gw_list_s *list)
{
	if(list == NULL)
	{
		writelog(ZLOG_ERR, "[chk_set_gwlist] Invalid argument!");
		return -1;		
	}
	int i=0;
	for(i=0;i<MAX_ERR_RESET_TIME;i++)
	{
		gw_node_s *n;
		n = list->Head;
		while(n != NULL)
		{
			if((n->gwID == 0)
				||((n->DevErr == 0)&&(n->AllSpaceDevErr == 0)))
			{
				n=n->Next;
				continue;
			}

			writelog(ZLOG_GW, "��������ID:%d,%s",n->gwID,n->gwIP);
			if(setinfo_gw(n, 1) == 0)/* ����ID��ʱ����Ϊ1 */
			{
				n->unConnTc = 0;
				n->DevErr=0;
				writelog(ZLOG_GW, "��������ID:%d,%s ���",n->gwID,n->gwIP);
			}
			else
			{
				n->unConnTc++;
				writelog(ZLOG_GW, "��������ʧ�� ID:%d,%s:%dʧ��,��%d��",
					n->gwID,n->gwIP,n->gwPort,n->unConnTc);
				if(n->unConnTc < 3)
				{
					n=n->Next;
					continue;
				}

				n->DevErr = GW_COMMU_ERR;		/* �������Ӵ��� */
				int i;			
				for(i=0; i<MAX_GWSPACE_LEN; i++)
				{
					if(n->SpaceInfo[i].SpaceID == 0)
					{
						continue;
					}
					n->SpaceInfo[i].DevErr=DETOR_COMMU_ERR;
				}
			}

			/* ���ÿһ����λ������Ƿ��������� */
			int i;	
			n->AllSpaceDevErr = 0;
			for(i=0; i<MAX_GWSPACE_LEN; i++)
			{
				if(n->SpaceInfo[i].DevErr != 0)
				{
					n->AllSpaceDevErr=DETOR_COMMU_ERR;
					break;
				}
			}
			
			n=n->Next;	
		}
	}
	
	return 0;
}




/*-----------------------------------------------------------------------
* ������  	: set_gwlist
* ����	  	: ���ͻ�������ʽ������������Ϸ����ж�,�ýӿ�����һ��������
            : �����AllSpaceDevErr����λֻ�����õ�ʱ����
* ����    	: @cmd : ������
			: @cmdID : ����ID
			: @content : ����
			: @len : ���ݳ���
		  
* ���	  : @buf : �������
* ����	  : 0: ����
			<0:�쳣
-------------------------------------------------------------------------*/
int get_spaceInfo2list(gw_list_s *list)
{
	if(list == NULL)
	{
		writelog(ZLOG_ERR, "[set_gwlist] Invalid argument!");
		return -1;		
	}

	gw_node_s *n;
	n = list->Head;
	while(n != NULL)
	{
		if(n->gwID == 0)
		{
			n=n->Next;
			continue;
		}
		if(getinfo_gw(n, 1) == 0)/* ����ID��ʱ����Ϊ1 */
		{
			n->unConnTc=0;
			n->DevErr = 0;
			writelog(ZLOG_GW, "��ȡ������Ϣ ID:%d,%s:%d���",n->gwID,n->gwIP,n->gwPort);
		}
		else
		{
			n->unConnTc++;
			writelog(ZLOG_GW, "��ȡ������Ϣ ID:%d,%s:%dʧ��,��%d��",
				n->gwID,n->gwIP,n->gwPort,n->unConnTc);
			if(n->unConnTc < 3) continue;

			n->DevErr = GW_COMMU_ERR;			/* ���ش��� */
			int i;			
			for(i=0; i<MAX_GWSPACE_LEN; i++)
			{
				if(n->SpaceInfo[i].SpaceID == 0)continue;
				n->SpaceInfo[i].DevErr=DETOR_COMMU_ERR;
			}
		}
		
		n=n->Next;	
	}
	return 0;
}


