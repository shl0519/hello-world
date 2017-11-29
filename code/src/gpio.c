#include "include.h"
#define ZLGGPI_NUM 8
#define ZLGGPO_NUM 8


//M3352
//static char gpo[8][12] = {"48","49", "50", "51", "52", "53", "54", "55"};
//_by_pbh_2014-11-07更改输出口的引脚排布方向，保证输入和输出IO口朝向一致

static char gpo[8][12] = {"55","54", "53", "52", "51", "50", "49", "48"};
static char gpi[8][12] = {"56", "57", "58", "59", "13", "12", "41", "40"};

void gpio_del(void);

/*******************************************************************************
* 函数名  : gpio_init
* 功能	  : gpio初始化
* 输入	  : 无
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
void gpio_init(void)
{
	int i = -1;
	char buf[256];

	/* 需要删除原来建立的IO口 */
	gpio_del();

	/* 建立输入口 */
	for( i = 0; i < ZLGGPI_NUM; i++)
	{
		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo ");
		strcat(buf, gpi[i]);
		strcat(buf, " > /sys/class/gpio/export");
//		printf("buf is: %s\n", buf);
		system(buf);

		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo in > ");
		strcat(buf, "/sys/class/gpio/gpio");
		strcat(buf, gpi[i]);
		strcat(buf, "/direction");
//		printf("buf is: %s\n", buf);
		system(buf);
	}

	/* 建立输出口并初始化输出0 */
	for( i = 0; i < ZLGGPO_NUM; i++)
	{
		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo ");
		strcat(buf, gpo[i]);
		strcat(buf, " > /sys/class/gpio/export");
//		printf("buf is: %s\n", buf);
		system(buf);

		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo out > ");
		strcat(buf, "/sys/class/gpio/gpio");
		strcat(buf, gpo[i]);
		strcat(buf, "/direction");
//		printf("buf is: %s\n", buf);
		system(buf);

		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo 0 > ");
		strcat(buf, "/sys/class/gpio/gpio");
		strcat(buf, gpo[i]);
		strcat(buf, "/value");
//		printf("buf is %s\n", buf);
		system(buf);
	}

}
/*******************************************************************************
* 函数名  : gpo_ctrl
* 功能	  : 控制输出口
* 输入	  : @outPortNO : 输入口编号
		  : @outputStat : 控制状态，0或1
* 输出	  : 无
* 返回	  : 无
*******************************************************************************/
void gpo_ctrl(unsigned char outPortNO,int outputStat)
{
	char buf[256];
	memset(buf, 0, sizeof(buf));
	strcat(buf, "echo ");

	if(outputStat == 0 )
	{
		strcat(buf, "0");
	}
	else if(outputStat == 1)
	{
		strcat(buf, "1");
	}
	strcat(buf, " > ");
	strcat(buf, "/sys/class/gpio/gpio");
	strcat(buf, gpo[outPortNO]);
	strcat(buf,"/");
	strcat(buf, "value");
	//printf("buffer is %s\n", buffer);
	system(buf);
}
/*******************************************************************************
* 函数名  : gpi_getstat
* 功能	  : 获取输入口状态
* 输入	  : @outPortNO : 输入口编号
		  : @outputStat : 控制状态，0或1
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
int gpi_getstat(unsigned char inPortNO)
{
	char buf[256];
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	strcat(buf, "/sys/class/gpio/gpio");
	strcat(buf, gpi[inPortNO]);
	strcat(buf, "/value");

	if( (fp = fopen(buf, "r")) == NULL )
	{
		writelog(ZLOG_ERR,"The %s was not exist,Error:%s,errno:%d",buf,strerror(errno),errno);
		writelog(ZLOG_SYS,"The %s was not exist,Error:%s,errno:%d",buf,strerror(errno),errno);
		writelog(ZLOG_SYS,"读取gpo错误，系统重启");
		sleep(1);
		reboot(RB_AUTOBOOT);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	fgets(buf, sizeof(buf)+1, fp);
	//printf("buf is %s\n", buf);
	fclose(fp);
	return atoi(buf);
}
/*******************************************************************************
* 函数名  : gpi_getstat
* 功能	  : 获取输入口状态
* 输入	  : @outPortNO : 输入口编号
		  : @outputStat : 控制状态，0或1
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/
void gpio_del(void)
{
	int i = -1;
	char buf[256];

	for( i = 0; i < ZLGGPI_NUM; i++)
	{
		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo ");
		strcat(buf, gpi[i]);
		strcat(buf, " > /sys/class/gpio/unexport");
//		printf("buf: is %s\n", buf);
		system(buf);
	}

	for( i = 0; i < ZLGGPO_NUM; i++)
	{
		memset(buf, 0, sizeof(buf));
		strcat(buf, "echo ");
		strcat(buf, gpo[i]);
		strcat(buf, " > /sys/class/gpio/unexport");
//		printf("buf: is %s\n", buf);
		system(buf);
	}
}





