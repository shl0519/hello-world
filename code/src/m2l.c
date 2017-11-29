#include "include.h"

int gm2lcmd[M2L_CMD_TOTAL]=
{
	REBOOT, 
	TIMING, 
	GET_SPACE_TOTAL, 
	GET_SPACE_DETAIL
};

/*******************************************************************************
* 函数名  : m2lmsg_anlys
* 功能	  : 管理机发送给本地的消息解析
* 输入	  : void *buf : 消息缓存区
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
int m2lmsg_anlys(char *buf,int len,char *content,int *res,int *cmd)
{
	if((buf==NULL)||(len==0)||(content == NULL))
	{
		writelog(ZLOG_ERR, "[m2lmsg_anlys] Invalid argument!");
		return -1;
	}

	int i,j;
	int clen;

	j=0;
	memcpy(&j,&buf[0],4);		/* 帧头 */
	if(j != 0x46464646)
	{
		writelog(ZLOG_ERR,"接收回应帧头错误");
		*res=FRAME_HEAD_ERR;
		return -1;
	}
	
	j=0;
	j=buf[4];					/* 命令字 */
	for(i=0; i<M2L_CMD_TOTAL; i++)
	{
		if(j == gm2lcmd[i])
		{ 
			break;	
		}
	}
	if(i >= M2L_CMD_TOTAL) //错误命令字
	{
		writelog(ZLOG_ERR,"没有该命令字:%02X",j);
		*res = FRAME_CMD_ERR;
		return -1;
	}
	*cmd=j;
	
	clen=0;
	clen=(buf[5] << 8) | buf[6];

	if(clen > 0)/* 长度不为0 */
	{
		memcpy(content,&buf[7],clen);	
		
	}
	content[clen]='\0';
	j=0;
	memcpy(&j,&buf[7+clen],4);		/* 帧头 */
	if(j != 0x64646464)
	{
		writelog(ZLOG_ERR,"接收回应帧尾错误");
		*res = FRAME_TAIL_ERR;
		return -1;
	}
	return 0;

}

/*******************************************************************************
* 函数名  : fmtrly_m2lmsg
* 功能	  : 格式化发送给管理机的帧数据
* 输入	  : void *buf : 消息缓存区
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
int fmtrly_m2lmsg(char *inbuf,int cmd,int execres,char *outbuf,int *outlen)
{
	if(outbuf == NULL)
	{
		writelog(ZLOG_ERR, "[fmtrly_m2lmsg] Invalid argument!");
		return -1;
	}

	outbuf[0]=0x46;
	outbuf[1]=0x46;
	outbuf[2]=0x46;
	outbuf[3]=0x46;

	outbuf[4]=(char)cmd;
	outbuf[5]=(char)execres;

	int len=0;
	if(inbuf != NULL)
	{
		len=strlen(inbuf);
		outbuf[6]=(char)(len&0x0000ff00)>>8;
		outbuf[7]=(char)len&0x000000ff;		
	}
	else 
	{
		len=0;
		outbuf[6]=0;
		outbuf[7]=0;	
	}

	if((len != 0)&&(inbuf != NULL))
	{
		memcpy(&outbuf[8],inbuf,len);
	}

	outbuf[8+len]=0x64;
	outbuf[9+len]=0x64;
	outbuf[10+len]=0x64;
	outbuf[11+len]=0x64;
	*outlen=len+12;
	return 0;

}

/*******************************************************************************
* 函数名  : get_spacetotal
* 功能	  : 获取车位总数并根据通信协议回复内容的格式产生内容
* 输入	  : glist,网关队列
* 输出	  : res,发送包的内容
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int get_spacetotal(gw_list_s* glist,char *res)
{
	if((glist == NULL) || (res == NULL))
	{
		writelog(ZLOG_ERR, "[get_spacetotal] Invalid argument!");
		return -1;
	}
	gw_node_s *n;
	n = glist->Head;
	
	int total=0;
	int remainTotal=0;
	int usrInUseTotal=0;//用户正在使用的车位数，表示车辆正停在车位上的数量
	int abnormal=0;     //异常总数
	int i;

	while(n != NULL)
	{
		if(n->gwID == 0)
		{
			n=n->Next;
			continue;
		}

		for(i=0;i<MAX_GWSPACE_LEN;i++)
		{
			if((n->SpaceInfo[i].SpaceID == 0)
				||(n->SpaceInfo[i].InUse == 0))
			{	
				continue;
			}
			total++;
			if(n->SpaceInfo[i].DevErr != 0)/* 异常总数,异常车位不参与计数 */
			{
				abnormal++;		
			}
			else if(n->SpaceInfo[i].InCnt != 0)/* 有参与计数的车位才能参与统计 */
			{
				if(n->SpaceInfo[i].NowStat == 0)
				{
					remainTotal++;
				}
				else
				{
					usrInUseTotal++;
				}	
			}	

		}
		n=n->Next;
	}

	sprintf(res,"%d;%d;%d;%d;%d;%d;",gwlist->ParkId, gwlist->GarageId,  
									total,remainTotal,usrInUseTotal,abnormal);
	return 0;
}


/*******************************************************************************
* 函数名  : get_spacedetail
* 功能	  : 获取车位明细，并根据通信协议格式产生回复帧的内容
* 输入	  : glist,网关队列
* 输出	  : res,发送包的内容
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int get_spacedetail(gw_list_s* glist,char *res)
{
	if((glist == NULL) || (res == NULL))
	{
		writelog(ZLOG_ERR, "[get_spacetotal] Invalid argument!");
		return -1;
	}
	gw_node_s *n;
	n = glist->Head;
	
	int i;	
	char tmpbuf[32];
	
	sprintf(res,"%d;%d;",gwlist->ParkId, gwlist->GarageId);
	
	while(n != NULL)
	{
		if(n->gwID == 0)
		{
			n=n->Next;
			continue;
		}

		for(i=0;i<MAX_GWSPACE_LEN;i++)
		{
			if((n->SpaceInfo[i].SpaceID == 0)
				||(n->SpaceInfo[i].InUse == 0))
			{	
				continue;
			}	

			int j;
			memset(tmpbuf,0,sizeof(tmpbuf));
			j= n->SpaceInfo[i].DevErr<<4 | n->SpaceInfo[i].NowStat;/* 加上错误码 */
			sprintf(tmpbuf,"%s;%d;",n->SpaceInfo[i].SpaceNO, j);
			strcat(res,tmpbuf);			
		}
		n=n->Next;
	}
	return 0;
}

/*******************************************************************************
* 函数名  : exec_m2lmsg
* 功能	  : 执行消息
* 输入	  : void *buf : 消息缓存区
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
int exec_m2lmsg(char *content,int cmd,char *res)
{
	if((content==NULL)||(res == NULL))
	{
		writelog(ZLOG_ERR, "[exec_m2lmsg] Invalid argument!");
		return -1;
	}

	switch(cmd)
	{
		case REBOOT:
		{
			writelog(ZLOG_MNG,"[收到重启设备命令]");
			//reboot(RB_AUTOBOOT);//重启指令不在这边执行，先回复执行成功，之后在执行
			break;
		}
		case TIMING:
		{
			writelog(ZLOG_MNG, "[收到修改系统时间]: %s",content);
			char buf[64];
			memset(buf, 0, sizeof(buf));
			strcat(buf, "date ");
			strcat(buf, content);
			system(buf);
			sleep(1);
			system("hwclock -w");

			break;	
		}
		case GET_SPACE_TOTAL:
		{
			writelog(ZLOG_MNG, "[收到获取车位总数指令]");
	
			/* 上网关锁 */
			pthread_cleanup_push(thread_free,(void*)&gwlist->Mtx);
			pthread_mutex_lock(&gwlist->Mtx);
			//pthread_cond_wait(&gwlist->Cond, &gwlist->Mtx);

			/* 获取总数 */
			get_spacetotal(gwlist,res);

			//解网关锁
			pthread_mutex_unlock(&gwlist->Mtx);
			//pthread_cond_signal(&gwlist->Cond);
			pthread_cleanup_pop(0);
			break;	
		}
		case GET_SPACE_DETAIL:
		{
			writelog(ZLOG_MNG, "[收到获取车位明细指令]");	
	
			/* 上网关锁 */
			pthread_cleanup_push(thread_free,(void*)&gwlist->Mtx);
			pthread_mutex_lock(&gwlist->Mtx);
			//pthread_cond_wait(&gwlist->Cond, &gwlist->Mtx);

			/* 获取明细，所有车位信息 */
			get_spacedetail(gwlist,res);

			//解网关锁
			pthread_mutex_unlock(&gwlist->Mtx);
			//pthread_cond_signal(&gwlist->Cond);
			pthread_cleanup_pop(0);
			break;	
		}
		default:
		{
			writelog(ZLOG_ERR, "不识别该命令:%08x",cmd);
			break;
		}
	}

	return 0;

}

