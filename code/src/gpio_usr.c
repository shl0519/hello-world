#include "include.h"

/* 方便IO读写 */
unsigned char const GPO_H[9]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0xFF};
unsigned char const GPO_L[9]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0x00};

const gponame GPO[8]={GPO1,GPO2,GPO3,GPO4,GPO5,GPO6,GPO7,GPO8};
const gpiname GPI[8]={GPI1,GPI2,GPI3,GPI4,GPI5,GPI6,GPI7,GPI8};

GPI_StatAndTime GPIStat[8];
static unsigned char gpo_reg=0;//gpo此次输出控制寄存器
/*******************************************************************************
* 函数名  : gpi_read
* 功能	  : 读取所有8个输入口
* 输入	  : 无
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
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
* 函数名  : gpo_write
* 功能	  : 写所有GPO
* 输入	  : 无
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
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
/* 如果外部短接，通过硬件结构，输入0到芯片 */
/* 如果外部断开，通过硬件结构，输入1到芯片 */
/* 常态是断开，有车压线圈时，外部会短接 */
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
		if(gpi_n->TimeCount>=GPIO_IntimalFilter_Time)/* GPIO内层采样次数 */
		{
			gpi_n->TimeCount=0x00;
			gpi_n->Lasttime=gpi_n->Thistime;

			struct	timeval t;
			gettimeofday(&t,NULL);

			if(gpi_n->Thistime == HIGHTLEVEL)
			{
			#if 0 //_by_shl_关掉外层滤波_20140401
				int j;							/* GPIO外层滤波时间 */
				j=(t.tv_sec - gpi_n->LEfficRedge_SysTime.tv_sec)*1000
					+ (t.tv_usec - gpi_n->LEfficRedge_SysTime.tv_usec)/1000;
				//printf("xxxxxxxxxxxxxxxxxxxxxx:%d\r\n",j);
				if(j<GPIO_OuterFilter_Time)
				{
					return;
				}
			#endif
				gpi_n->LEfficRedge_SysTime=t;	/* 保存有效上升沿时间 */

				gpi_n->State=HIGHTLEVEL;
			}
			else
			{
			#if 0 //_by_shl_关掉外层滤波_20140401
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
* 函数名  : readgpi_machupcfg
* 功能	  : 读取GPI状态，写入GPI对象
* 输入	  : 无
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
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
* DESCRIPTION:_by_pbh_gpo寄存器:WriteGPO_REG_MachUpCFG该函数接口在多线程中使用不安全_20150706

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
			else if(((j&0xf0)!=0)&&((j&0x0f)!=0))gpo_reg=gpo_reg&j;//表示是写低电平
			else gpo_reg=gpo_reg|j;//表示是写高电平

			break;
		}
	}
}

void wrgpo_machupcfg(void)
{
    static unsigned char m_reg = 0;  //gpo上一次输出控制寄存器
    unsigned char reg = gpo_reg;//_by_pbh_gpo寄存器:执行IO输出时，先保存输出值gpo_reg到函数局部变量reg，防止输出值被意外修改，影响输出功能_20150706

	if(m_reg != reg){
		gpo_write(reg);
		m_reg = reg;
	}
}

/*******************************************************************************
* 函数名  : snapcoil_edgeporc
* 功能	  : gpi事件处理
* 输入	  : 无
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
void snapcoil_edgeporc(GPI_StatAndTime *snap,int time)
{
	static int timecnt;

	
	if((snap->LState==LOWLEVEL)&&(snap->State==HIGHTLEVEL))
	{
		writelog(ZLOG_LANE, "车辆进入抓拍线圈");	
             if(reminspace > 0)
             {
                //控制栏杆抬杠
                timecnt=0;
                wrgpo_reg_machupcfg(SysCfg.RailUp,UPRAIL);
                writelog(ZLOG_LANE, "栏杆抬杆脉冲触发开始");
             }
             else  writelog(ZLOG_LANE, "剩余车位位0，不允许进场");
	}
	else if((snap->LState==HIGHTLEVEL)&&(snap->State==HIGHTLEVEL))
	{
	      if(reminspace > 0)
            {   
        		timecnt++;
        		if(timecnt == (time+1))
        		{
        			wrgpo_reg_machupcfg(SysCfg.RailUp,RELEASE_UPRAIL);
        			writelog(ZLOG_LANE, "栏杆抬杆脉冲触发结束");
        		}
             }
	}
	else if((snap->LState==HIGHTLEVEL)&&(snap->State==LOWLEVEL))
	{
		writelog(ZLOG_LANE, "车辆离开抓拍线圈");	
 	      if(reminspace > 0)
            {          
        		timecnt=0;
        		wrgpo_reg_machupcfg(SysCfg.RailUp,RELEASE_UPRAIL);
             }

	}
}

/*******************************************************************************
* 函数名  : gpiedge_proc
* 功能	  : gpi事件处理
* 输入	  : 无
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
void gpiedge_proc(void)
{
	unsigned char i;
      /* 20170221 修改成剩余多少个车位就不允许车辆进入 */
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

