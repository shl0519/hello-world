#include "include.h"

sys_config_s SysCfg;


int delfile(char* pathfile)
{
	int j;
	j=remove(pathfile);
	if(j!=0)
	{
		writelog(ZLOG_ERR,"删除文件%s失败",pathfile);
		return -1;
	}
	writelog(ZLOG_SYS,"删除文件%s成功",pathfile);
	return 0;
}

/*******************************************************************************
* 函数名    : wr_syscfg
* 功能		: 把读取配置文件的信息写入到内存中
* 输入		: buf,文件一行的内容
* 输出		:
* 返回		: 无
*******************************************************************************/
void wr_syscfg(char *buf)
{
	char item_name[50];
	char item_value[50];
	int i,j;
	memset(item_name,0,sizeof(item_name));
	memset(item_value,0,sizeof(item_value));

	for(i=0;i<50;i++)
	{
		if(buf[i] != '=')item_name[i]=buf[i];
		else
		{
			item_name[i]='\0';
			break;
		}
	}
	//---读取配置项目的配置

	i=i+1;
	j=0;
	for(;i<100;i++)
	{
		//if((buffer[i]!='\r')&&(buffer[i]!='\0'))item_value[j]=buffer[i];//回车符和结束符
		if(buf[i]!=';')item_value[j]=buf[i];
		else
		{
			item_value[j]='\0';
			break;
		}
		j++;
	}

	if(strcmp(item_name,"本地IP")==0)			 		strcpy(SysCfg.Loc.IP,item_value);
	else if(strcmp(item_name,"本地通信网关")==0) 		strcpy(SysCfg.Loc.GateWay,item_value);
	else if(strcmp(item_name,"本地消息接收端口")==0) 	SysCfg.Loc.MsgPort = atoi(item_value);
	else if(strcmp(item_name,"本地文件接收端口")==0) 	SysCfg.Loc.FilePort = atoi(item_value);

	else if(strcmp(item_name,"管理机IP")==0) 			strcpy(SysCfg.Mng.IP,item_value);
	else if(strcmp(item_name,"管理机消息接收端口")==0) 	SysCfg.Mng.MsgPort = atoi(item_value);
	else if(strcmp(item_name,"管理机文件接收端口")==0) 	SysCfg.Mng.FilePort = atoi(item_value);
	else if(strcmp(item_name,"是否配置3G模块")==0) 		SysCfg.Modul_3G = atoi(item_value);

	else if(strcmp(item_name,"车位已满实剩车位数")==0)  SysCfg.SpaceFull_RealRmn = atoi(item_value);

	else if(strcmp(item_name,"入口抓拍线圈输入口")==0) 	SysCfg.SnapCoil = atoi(item_value);
	else if(strcmp(item_name,"入口控制抬杆输出口")==0) 	SysCfg.RailUp = atoi(item_value);
	/* 输出抬杆信号脉冲宽度，单位以毫秒计算 */
	else if(strcmp(item_name,"输出抬杆信号脉冲宽度")==0) SysCfg.RailupPulseWidth = atoi(item_value);

	else if(strcmp(item_name,"上传用户IP")==0)			 		strcpy(SysCfg.UsrCommn.IP,item_value);
      else if(strcmp(item_name,"上传用户端口")==0)			 	SysCfg.UsrCommn.MsgPort = atoi(item_value);
      else if(strcmp(item_name,"上传用户网关")==0)			 	strcpy(SysCfg.UsrCommn.GateWay,item_value);


}

/*******************************************************************************
* name   : set_defaultSysCfg
* 功能		: 设置系统默认参数
* 输入		: no
* 输出		: no
* 返回		: no
*******************************************************************************/
void set_defaultSysCfg(void)
{
	strcpy(SysCfg.Loc.IP,"192.168.1.101");
	SysCfg.Loc.MsgPort=9812;
	SysCfg.Loc.FilePort=9813;
	strcpy(SysCfg.Loc.GateWay,"192.168.1.1");

	strcpy(SysCfg.Mng.IP,"192.168.1.131");
	SysCfg.Mng.MsgPort=9912;
	SysCfg.Mng.FilePort=9913;
	
	SysCfg.Modul_3G=0;
	
	SysCfg.SpaceFull_RealRmn = 2;

	SysCfg.SnapCoil = 1;
	SysCfg.RailUp = 1;
	
	SysCfg.RailupPulseWidth = 200;

}

/*******************************************************************************
* 函数名    : rd_syscfg
* 功能		: 读取系统配置信息
* 输入		: 无
* 输出		:
* 返回		: 初始化完的队列管理
*******************************************************************************/
void rd_syscfg(void)
{
	FILE *fp;
	
	memset(&SysCfg,0,sizeof(sys_config_s));
	
	if((fp=fopen(PATH_SYS_CONFIG, "r")) != NULL)
	{	
		char buf[256]={0};
		while(!feof(fp))
		{
			memset(buf,0,sizeof(buf));
			fgets(buf,sizeof(buf)+1,fp); //读取了一行(包括行尾的'\r\n',并自动加上字符串结束符'\0')。
			//printf("%s\r\n",buf);
			wr_syscfg(buf);
		}
		fclose(fp);
	}
	else
	{
		writelog(ZLOG_ERR,"配置文件%s 不存在，请检查!Error:%s,errno:%d\r\n",PATH_SYS_CONFIG,strerror(errno),errno);
		set_defaultSysCfg();
	}
	
	/* 设置本地IP地址和子网掩码 */
       set_LocalNetInfo("eth0",SysCfg.Loc.IP,"255.255.255.0",SysCfg.Loc.GateWay);
        //_shl_20161225_对接信息港
       set_LocalNetInfo("eth0:0","172.16.42.74","255.255.0.0",SysCfg.UsrCommn.GateWay);
	if(SysCfg.Modul_3G==1)
	{
		set_LocalNetInfo("eth0:1","192.168.1.21","255.255.255.0","192.168.1.2");
	}
	
}




