#include "include.h"

/* ����IO��д */
unsigned char const GPO_H[9]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0xFF};
unsigned char const GPO_L[9]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0x00};

const gponame GPO[8]={GPO1,GPO2,GPO3,GPO4,GPO5,GPO6,GPO7,GPO8};
const gpiname GPI[8]={GPI1,GPI2,GPI3,GPI4,GPI5,GPI6,GPI7,GPI8};

GPI_StatAndTime GPIStat[8];
static unsigned char gpo_reg=0;//gpo�˴�������ƼĴ���
/*******************************************************************************
* ������  : gpi_read
* ����	  : ��ȡ����8�������
* ����	  : ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
static unsigned char gpi_read(void)
{
	int i;
	unsigned char j,k;
	j=0;
	k=0;
	for(i=0;i<8;i++)
	{
		j=gpi_getstat(i);
		k=k|(j<<i);
	}
	return k;
}
/*******************************************************************************
* ������  : gpo_write
* ����	  : д����GPO
* ����	  : ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
static int gpo_write(const unsigned char WriteGPIO_Date)
{
	unsigned char writep=0;
	writep=WriteGPIO_Date;

	int i;
	for(i=0;i<8;i++)
	{
		if((writep&0x01)==1)gpo_ctrl(i,1);
		else gpo_ctrl(i,0);
		writep=writep>>1;
	}

	return 0;
}


/******************************************************************************/
/* ����ⲿ�̽ӣ�ͨ��Ӳ���ṹ������0��оƬ */
/* ����ⲿ�Ͽ���ͨ��Ӳ���ṹ������1��оƬ */
/* ��̬�ǶϿ����г�ѹ��Ȧʱ���ⲿ��̽� */
/******************************************************************************/
static void readgpi_proc(unsigned char gpi_stat,GPI_StatAndTime *gpi_n)
{
	gpi_n->LState=gpi_n->State;
	if(gpi_stat==0)
	{
		gpi_n->Thistime=HIGHTLEVEL;
	}
	else gpi_n->Thistime=LOWLEVEL;
	if(gpi_n->Thistime != gpi_n->Lasttime)
	{
		gpi_n->TimeCount++;
		if(gpi_n->TimeCount>=GPIO_IntimalFilter_Time)/* GPIO�ڲ�������� */
		{
			gpi_n->TimeCount=0x00;
			gpi_n->Lasttime=gpi_n->Thistime;

			struct	timeval t;
			gettimeofday(&t,NULL);

			if(gpi_n->Thistime == HIGHTLEVEL)
			{
			#if 0 //_by_shl_�ص�����˲�_20140401
				int j;							/* GPIO����˲�ʱ�� */
				j=(t.tv_sec - gpi_n->LEfficRedge_SysTime.tv_sec)*1000
					+ (t.tv_usec - gpi_n->LEfficRedge_SysTime.tv_usec)/1000;
				//printf("xxxxxxxxxxxxxxxxxxxxxx:%d\r\n",j);
				if(j<GPIO_OuterFilter_Time)
				{
					return;
				}
			#endif
				gpi_n->LEfficRedge_SysTime=t;	/* ������Ч������ʱ�� */

				gpi_n->State=HIGHTLEVEL;
			}
			else
			{
			#if 0 //_by_shl_�ص�����˲�_20140401
				int j;
				j=(t.tv_sec - gpi_n->LEfficFedge_SysTime.tv_sec)*1000
					+ (t.tv_usec - gpi_n->LEfficFedge_SysTime.tv_usec)/1000;
				//printf("xxxxxxxxxxxxxxxxxxxxxx:%d\r\n",j);
				if(j<GPIO_OuterFilter_Time)
				{
					return;
				}
			#endif
				gpi_n->LEfficFedge_SysTime=t;

				gpi_n->State=LOWLEVEL;

			}
		}
	}
	else
	{
		gpi_n->Lasttime=gpi_n->Thistime;
		gpi_n->TimeCount=0x00;
	}
}
/*******************************************************************************
* ������  : readgpi_machupcfg
* ����	  : ��ȡGPI״̬��д��GPI����
* ����	  : ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
void readgpi_machupcfg(void)
{
	unsigned char i,j,m;
	j=0;
	j=gpi_read();
	for(i=0;i<8;i++)
	{
		m=j&(0x01<<i);
		readgpi_proc(m,&GPIStat[i]);
	}
}

/*****************************************************************************
* WriteGPO_REG_MachUpCFG
* DESCRIPTION:_by_pbh_gpo�Ĵ���:WriteGPO_REG_MachUpCFG�ú����ӿ��ڶ��߳���ʹ�ò���ȫ_20150706

* @Param  : null
* @Return : null
* ----
******************************************************************************/
void wrgpo_reg_machupcfg(gponame OutputCfg,bool level)
{
	unsigned char i;
	unsigned char j;

	for(i=0;i<8;i++){
		if(GPO[i]==OutputCfg){
			if(level==1)j=GPO_H[i];
			else j=GPO_L[i];

			if((j==0xff)||(j==0x00))gpo_reg=j;
			else if(((j&0xf0)!=0)&&((j&0x0f)!=0))gpo_reg=gpo_reg&j;//��ʾ��д�͵�ƽ
			else gpo_reg=gpo_reg|j;//��ʾ��д�ߵ�ƽ

			break;
		}
	}
}

void wrgpo_machupcfg(void)
{
    static unsigned char m_reg = 0;  //gpo��һ��������ƼĴ���
    unsigned char reg = gpo_reg;//_by_pbh_gpo�Ĵ���:ִ��IO���ʱ���ȱ������ֵgpo_reg�������ֲ�����reg����ֹ���ֵ�������޸ģ�Ӱ���������_20150706

	if(m_reg != reg){
		gpo_write(reg);
		m_reg = reg;
	}
}

/*******************************************************************************
* ������  : snapcoil_edgeporc
* ����	  : gpi�¼�����
* ����	  : ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
void snapcoil_edgeporc(GPI_StatAndTime *snap,int time)
{
	static int timecnt;

	
	if((snap->LState==LOWLEVEL)&&(snap->State==HIGHTLEVEL))
	{
		writelog(ZLOG_LANE, "��������ץ����Ȧ");	
             if(reminspace > 0)
             {
                //��������̧��
                timecnt=0;
                wrgpo_reg_machupcfg(SysCfg.RailUp,UPRAIL);
                writelog(ZLOG_LANE, "����̧�����崥����ʼ");
             }
             else  writelog(ZLOG_LANE, "ʣ�೵λλ0�����������");
	}
	else if((snap->LState==HIGHTLEVEL)&&(snap->State==HIGHTLEVEL))
	{
	      if(reminspace > 0)
            {   
        		timecnt++;
        		if(timecnt == (time+1))
        		{
        			wrgpo_reg_machupcfg(SysCfg.RailUp,RELEASE_UPRAIL);
        			writelog(ZLOG_LANE, "����̧�����崥������");
        		}
             }
	}
	else if((snap->LState==HIGHTLEVEL)&&(snap->State==LOWLEVEL))
	{
		writelog(ZLOG_LANE, "�����뿪ץ����Ȧ");	
 	      if(reminspace > 0)
            {          
        		timecnt=0;
        		wrgpo_reg_machupcfg(SysCfg.RailUp,RELEASE_UPRAIL);
             }

	}
}

/*******************************************************************************
* ������  : gpiedge_proc
* ����	  : gpi�¼�����
* ����	  : ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
void gpiedge_proc(void)
{
	unsigned char i;
      /* 20170221 �޸ĳ�ʣ����ٸ���λ�Ͳ����������� */
	for(i=0;i<8;i++)
	{
		if(GPI[i] == SysCfg.SnapCoil)
		{
			//snapcoil_edgeporc(&GPIStat[i],SysCfg.RailupPulseWidth/30);
			snapcoil_edgeporc(&GPIStat[i],30);
			break;
		}
	}
}

