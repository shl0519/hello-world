#include "include.h"

sys_config_s SysCfg;


int delfile(char* pathfile)
{
	int j;
	j=remove(pathfile);
	if(j!=0)
	{
		writelog(ZLOG_ERR,"ɾ���ļ�%sʧ��",pathfile);
		return -1;
	}
	writelog(ZLOG_SYS,"ɾ���ļ�%s�ɹ�",pathfile);
	return 0;
}

/*******************************************************************************
* ������    : wr_syscfg
* ����		: �Ѷ�ȡ�����ļ�����Ϣд�뵽�ڴ���
* ����		: buf,�ļ�һ�е�����
* ���		:
* ����		: ��
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
	//---��ȡ������Ŀ������

	i=i+1;
	j=0;
	for(;i<100;i++)
	{
		//if((buffer[i]!='\r')&&(buffer[i]!='\0'))item_value[j]=buffer[i];//�س����ͽ�����
		if(buf[i]!=';')item_value[j]=buf[i];
		else
		{
			item_value[j]='\0';
			break;
		}
		j++;
	}

	if(strcmp(item_name,"����IP")==0)			 		strcpy(SysCfg.Loc.IP,item_value);
	else if(strcmp(item_name,"����ͨ������")==0) 		strcpy(SysCfg.Loc.GateWay,item_value);
	else if(strcmp(item_name,"������Ϣ���ն˿�")==0) 	SysCfg.Loc.MsgPort = atoi(item_value);
	else if(strcmp(item_name,"�����ļ����ն˿�")==0) 	SysCfg.Loc.FilePort = atoi(item_value);

	else if(strcmp(item_name,"�����IP")==0) 			strcpy(SysCfg.Mng.IP,item_value);
	else if(strcmp(item_name,"�������Ϣ���ն˿�")==0) 	SysCfg.Mng.MsgPort = atoi(item_value);
	else if(strcmp(item_name,"������ļ����ն˿�")==0) 	SysCfg.Mng.FilePort = atoi(item_value);
	else if(strcmp(item_name,"�Ƿ�����3Gģ��")==0) 		SysCfg.Modul_3G = atoi(item_value);

	else if(strcmp(item_name,"��λ����ʵʣ��λ��")==0)  SysCfg.SpaceFull_RealRmn = atoi(item_value);

	else if(strcmp(item_name,"���ץ����Ȧ�����")==0) 	SysCfg.SnapCoil = atoi(item_value);
	else if(strcmp(item_name,"��ڿ���̧�������")==0) 	SysCfg.RailUp = atoi(item_value);
	/* ���̧���ź������ȣ���λ�Ժ������ */
	else if(strcmp(item_name,"���̧���ź�������")==0) SysCfg.RailupPulseWidth = atoi(item_value);

	else if(strcmp(item_name,"�ϴ��û�IP")==0)			 		strcpy(SysCfg.UsrCommn.IP,item_value);
      else if(strcmp(item_name,"�ϴ��û��˿�")==0)			 	SysCfg.UsrCommn.MsgPort = atoi(item_value);
      else if(strcmp(item_name,"�ϴ��û�����")==0)			 	strcpy(SysCfg.UsrCommn.GateWay,item_value);


}

/*******************************************************************************
* name   : set_defaultSysCfg
* ����		: ����ϵͳĬ�ϲ���
* ����		: no
* ���		: no
* ����		: no
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
* ������    : rd_syscfg
* ����		: ��ȡϵͳ������Ϣ
* ����		: ��
* ���		:
* ����		: ��ʼ����Ķ��й���
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
			fgets(buf,sizeof(buf)+1,fp); //��ȡ��һ��(������β��'\r\n',���Զ������ַ���������'\0')��
			//printf("%s\r\n",buf);
			wr_syscfg(buf);
		}
		fclose(fp);
	}
	else
	{
		writelog(ZLOG_ERR,"�����ļ�%s �����ڣ�����!Error:%s,errno:%d\r\n",PATH_SYS_CONFIG,strerror(errno),errno);
		set_defaultSysCfg();
	}
	
	/* ���ñ���IP��ַ���������� */
       set_LocalNetInfo("eth0",SysCfg.Loc.IP,"255.255.255.0",SysCfg.Loc.GateWay);
        //_shl_20161225_�Խ���Ϣ��
       set_LocalNetInfo("eth0:0","172.16.42.74","255.255.0.0",SysCfg.UsrCommn.GateWay);
	if(SysCfg.Modul_3G==1)
	{
		set_LocalNetInfo("eth0:1","192.168.1.21","255.255.255.0","192.168.1.2");
	}
	
}




