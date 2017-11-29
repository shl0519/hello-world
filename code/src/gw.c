#include "include.h"

/*----------------------------------------------------
* 函数名  	: sendbuf_fmt
* 功能	  	: 发送缓冲区格式化，包含命令合法性判断
* 输入    	: @cmd : 命令字
			: @cmdID : 命令ID
			: @content : 内容
			: @len : 内容长度
		  
* 输出	  : @buf : 输出缓存
* 返回	  : 0: 正常
			<0:异常
----------------------------------------------------*/
int sendbuf_fmt(unsigned int cmd, unsigned int cmdID, 
				unsigned char *content,unsigned len,unsigned char *buf,int *outlen)
{

	if((cmd != SET_GW_INFO)&&(cmd != GET_GW_RES))
	{
		writelog(ZLOG_ERR, "命令字错误:%08x",cmd);
		return -1;
	}
	if(buf == NULL)
	{
		writelog(ZLOG_ERR, "出口参数错误");
		return -1;	
	}
	unsigned int calcrc;/* 计算出CRC结果 */
	
	buf[0]=0xAA;
	buf[1]=0xAA;
	buf[2]=0xAA;
	buf[3]=0xAA;

	uint2ubuf(cmd,&buf[4]);				/* 命令字 */
	uint2ubuf(cmdID,&buf[8]);			/* ID */
	uint2ubuf(len,&buf[12]);			/* 内容长度 */
	if(content != NULL)
	{
		memcpy(&buf[16],content,len);	/* 内容 */
	}


	StringCrc32(&buf[4],&calcrc,len+12);/* 计算CRC32 */
	uint2ubuf(calcrc,(unsigned char *)&buf[len+16]);

	buf[len+20]=0x55;
	buf[len+21]=0x55;
	buf[len+22]=0x55;
	buf[len+23]=0x55;
	*outlen=len+24;
	return 0;
}

/*----------------------------------------------------
* 函数名  : content_alys
* 功能	  : 接受格式解析
* 输入    : @ip : 串口转网口设备的IP
		  : @port : 端口
		  : @money : 交易金额
		  : @paynumber :交易序号

* 输出	  : 无
* 返回	  : 0: 正常
			<0:异常
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
			if(len != MAX_GWSPACE_LEN)				/* 返回长度错误 */
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
					writelog(ZLOG_GW, "网关:%s  车位%s 异常",n->gwIP,n->SpaceInfo[i].SpaceNO);
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
			if(len != MAX_GWSPACE_LEN)				/* 返回长度错误 */
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
					writelog(ZLOG_GW, "网关:%s  车位%s 异常",n->gwIP,n->SpaceInfo[i].SpaceNO);
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
			writelog(ZLOG_ERR, "不识别该命令:%08x",cmd);
			return -1;
			break;
		}
	}
	return 0;
}
/*----------------------------------------------------
* 函数名  : recv_alys
* 功能	  : 接受格式解析
* 输入    : @ip : 串口转网口设备的IP
		  : @port : 端口
		  : @money : 交易金额
		  : @paynumber :交易序号

* 输出	  : 无
* 返回	  : 0: 正常
			<0:异常
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
	memcpy(&j,&buf[0],4);		/* 接收回应帧头 */
	if(j != 0xAAAAAAAA)
	{
		writelog(ZLOG_ERR,"接收回应帧头错误");
		return -1;
	}
	
	j=0;
	memcpy(&j,&buf[len-4],4);	/* 接收回应帧尾 */
	if(j != 0x55555555)
	{
		writelog(ZLOG_ERR,"接收回应帧尾错误");
		return -1;
	}

	icmd=0;
	ubuf2uint(&buf[4],&icmd);	/* 命令字 */	
	if(icmd != cmd)
	{
		writelog(ZLOG_ERR,"命令字不一致");
		return -1;
	}

	icmdID=0;
	ubuf2uint(&buf[8],&icmdID);	/* 命令ID */
	if(icmdID != cmdID)
	{
		writelog(ZLOG_ERR,"命令ID不一致");
		return -1;
	}

	execres=0;
	ubuf2uint(&buf[12],&execres);	/* 执行结果 */
	if(execres != EXEC_SUCCESS)
	{
		writelog(ZLOG_ERR,"执行结果错误");
		return -1;
	}

	icrc=0;
	ubuf2uint(&buf[len-8],&icrc);/* CRC校验 */
	StringCrc32(&buf[4],&calcrc,len-12);
	printf("-------------:%x,,,%x\r\n",icrc,calcrc);
	if(icrc != calcrc)
	{
		writelog(ZLOG_ERR,"CRC校验不一致");
		return -1;
	}

	ilen=0;
	ubuf2uint(&buf[16],&ilen);	/* 内容长度 */
	j=0;
	j=content_alys(&buf[20],ilen,icmd,n);
	if(j != 0)
	{
		writelog(ZLOG_ERR,"接收内容处理错误");
		return -1;
	}	

	return 0;

}

/*-----------------------------------------------------------------------
* 函数名  	: sendbuf_fmt
* 功能	  	: 发送缓冲区格式化，包含命令合法性判断,该接口属于一次性连接
* 输入    	: @cmd : 命令字
			: @cmdID : 命令ID
			: @content : 内容
			: @len : 内容长度
		  
* 输出	  : @buf : 输出缓存
* 返回	  : 0: 正常
			<0:异常
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
	if( sendlen != outlen)/* 发送采集明细指令是的包长度是20 */
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
	tcp_blkClose(&fd);//正常关闭返回
	
	memset(printbuf,0,sizeof(printbuf));
	fmt2str_msg(buf,j,printbuf);
	writelog(ZLOG_GW, "[getinfo_gw] get reply : %s success", printbuf);

	return(recv_alys(buf,j,GET_GW_RES,cmdID,(void*)n));
}
/*-----------------------------------------------------------------------
* 函数名  	: sendbuf_fmt
* 功能	  	: 发送缓冲区格式化，包含命令合法性判断,该接口属于一次性连接
* 输入    	: @cmd : 命令字
			: @cmdID : 命令ID
			: @content : 内容
			: @len : 内容长度
		  
* 输出	  : @buf : 输出缓存
* 返回	  : 0: 正常
			<0:异常
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

	if( sendlen != outlen)/* 发送采集明细指令是的包长度是20 */
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
	tcp_blkClose(&fd);//正常关闭返回0

	fmt2str_msg(buf,j,printbuf);
	writelog(ZLOG_GW, "[setinfo_gw] get reply : %s success", printbuf);

	return(recv_alys(buf, j, SET_GW_INFO, cmdID, n));
}

/*-----------------------------------------------------------------------
* 函数名  	: chkset_gwlist
* 功能	  	: 检查设置网关,超过3次设置不成功，那么默认成功
* 输入    	: @cmd : 命令字
			: @cmdID : 命令ID
			: @content : 内容
			: @len : 内容长度
		  
* 输出	  : @buf : 输出缓存
* 返回	  : 0: 正常
			<0:异常
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

			writelog(ZLOG_GW, "设置网关ID:%d,%s",n->gwID,n->gwIP);
			if(setinfo_gw(n, 1) == 0)/* 命令ID暂时设置为1 */
			{
				n->unConnTc = 0;
				n->DevErr=0;
				writelog(ZLOG_GW, "设置网关ID:%d,%s 完成",n->gwID,n->gwIP);
			}
			else
			{
				n->unConnTc++;
				writelog(ZLOG_GW, "设置网关失败 ID:%d,%s:%d失败,第%d次",
					n->gwID,n->gwIP,n->gwPort,n->unConnTc);
				if(n->unConnTc < 3)
				{
					n=n->Next;
					continue;
				}

				n->DevErr = GW_COMMU_ERR;		/* 网关连接错误 */
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

			/* 检查每一个车位检测器是否设置正常 */
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
* 函数名  	: set_gwlist
* 功能	  	: 发送缓冲区格式化，包含命令合法性判断,该接口属于一次性连接
            : 不检查AllSpaceDevErr，该位只在设置的时候检查
* 输入    	: @cmd : 命令字
			: @cmdID : 命令ID
			: @content : 内容
			: @len : 内容长度
		  
* 输出	  : @buf : 输出缓存
* 返回	  : 0: 正常
			<0:异常
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
		if(getinfo_gw(n, 1) == 0)/* 命令ID暂时设置为1 */
		{
			n->unConnTc=0;
			n->DevErr = 0;
			writelog(ZLOG_GW, "获取网关信息 ID:%d,%s:%d完成",n->gwID,n->gwIP,n->gwPort);
		}
		else
		{
			n->unConnTc++;
			writelog(ZLOG_GW, "获取网关信息 ID:%d,%s:%d失败,第%d次",
				n->gwID,n->gwIP,n->gwPort,n->unConnTc);
			if(n->unConnTc < 3) continue;

			n->DevErr = GW_COMMU_ERR;			/* 网关错误 */
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


