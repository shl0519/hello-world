#include "include.h"
#define ZLGGPI_NUM 8
#define ZLGGPO_NUM 8


//M3352
//static char gpo[8][12] = {"48","49", "50", "51", "52", "53", "54", "55"};
//_by_pbh_2014-11-07��������ڵ������Ų����򣬱�֤��������IO�ڳ���һ��

static char gpo[8][12] = {"55","54", "53", "52", "51", "50", "49", "48"};
static char gpi[8][12] = {"56", "57", "58", "59", "13", "12", "41", "40"};

void gpio_del(void);

/*******************************************************************************
* ������  : gpio_init
* ����	  : gpio��ʼ��
* ����	  : ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
void gpio_init(void)
{
	int i = -1;
	char buf[256];

	/* ��Ҫɾ��ԭ��������IO�� */
	gpio_del();

	/* ��������� */
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

	/* ��������ڲ���ʼ�����0 */
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
* ������  : gpo_ctrl
* ����	  : ���������
* ����	  : @outPortNO : ����ڱ��
		  : @outputStat : ����״̬��0��1
* ���	  : ��
* ����	  : ��
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
* ������  : gpi_getstat
* ����	  : ��ȡ�����״̬
* ����	  : @outPortNO : ����ڱ��
		  : @outputStat : ����״̬��0��1
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
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
		writelog(ZLOG_SYS,"��ȡgpo����ϵͳ����");
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
* ������  : gpi_getstat
* ����	  : ��ȡ�����״̬
* ����	  : @outPortNO : ����ڱ��
		  : @outputStat : ����״̬��0��1
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
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





