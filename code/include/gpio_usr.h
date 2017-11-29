#ifndef __GPIO_USR_H__
#define __GPIO_USR_H__

#define GPIO_IntimalFilter_Time 3	/* GPIO内层采样次数 */
#define GPIO_OuterFilter_Time 100	/* GPIO外层滤波时间,单位ms */

typedef  enum {
	GPO_UNCFG=0x00,
	GPO1=0x01,
	GPO2=0x02,
	GPO3=0x03,
	GPO4=0x04,
	GPO5=0x05,
	GPO6=0x06,
	GPO7=0x07,
	GPO8=0x08,
}gponame;

typedef  enum {
	GPI_UNCFG=0x00,
	GPI1=0x01,
	GPI2=0x02,
	GPI3=0x03,
	GPI4=0x04,
	GPI5=0x05,
	GPI6=0x06,
	GPI7=0x07,
	GPI8=0x08,
}gpiname;

typedef  enum 
{
	LOWLEVEL=0,
	HIGHTLEVEL=1,
}bool;

typedef  struct {
	bool Lasttime;
	bool Thistime;
	bool State;
	bool LState;
	int TimeCount;
	struct	timeval LEfficRedge_SysTime;	/* rising edge */
	struct	timeval LEfficFedge_SysTime;	/* failling edge */
}GPI_StatAndTime;



#define UPRAIL 1			//抬杆
#define RELEASE_UPRAIL 0	//抬杆释放














#endif

