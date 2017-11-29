/*********************************************************************************************
#####		  厦门路桥信息工程有限公司		  #####
#####			   嵌入式开发小组			  #####
#####										  #####

* File：	usergpio_test.c
* Author:	Hailiang.su
* Desc：	linux -- park contrl system
* History:	2013-08-09 16:36
*********************************************************************************************/

#include "include.h"



gw_list_s* gwlist;
msg_list_s *msglist;

char gledip[20];
int gRemin;


int main(int argc,char *argv[])
{
	/* 日志初始化 */
	ZlogInit();

	/* 清理空间,开机检查 */
	int cpu=0;
	int mem=0;
	int timecnt=0;
      gRemin=0;
        
	clrfile(USR_PATH_TPM,".txt");
	keep_Space(USR_PATH_HIS_LOG,MAX_SPACE_LOG_HIS,"*");
	keep_Space(USR_PATH_INT_LOG,MAX_SPACE_LOG_INT,"*");
	get_cpumen_info(&cpu,&mem);
	writelog(ZLOG_SYS,"正在获取CPU使用率和内存使用率......");
	writelog(ZLOG_SYS,"开机cpu使用率是:%d%%,内存使用率是:%d%%",cpu,mem);

	if((cpu > MAX_ALLOW_CPU_USR)||(MAX_ALLOW_MEM_USR > 90) )
	{
		writelog(ZLOG_SYS,"系统资源使用率偏高，该平台存在系统问题，请排查");
		sleep(1);
		reboot(RB_AUTOBOOT);
		return -1;
	}
	/* IO初始化 */
	gpio_init();
	
	/* 读取配置 */
	rd_syscfg();
	
	/* 数据库初始化 */
	database_init();

	/* 网关队列初始化 */
	gwlist = init_gwlist();

	//rdtxt_instab(TABNO_PARKINFO);
	//rdtxt_instab(TABNO_AREAINFO);


	/* 从数据库中获取网关配置信息获取 */
	full_gwlistheadinfo(TABNO_PARKINFO, gwlist);
	get_gwID2list(TABNO_AREAINFO, gwlist);
	full_gwlistinfo(gwlist);

	//strcpy(gledip,"210.39.2.143");
	//setLED_num((unsigned char*)gledip,5005,(unsigned char*)"15");
       // while(1);
	/* 配置网关 */

       /* 71号地库不设置网关 */
	//chkset_gwlist(gwlist);

	/* 消息队列初始化 */
	msglist = init_msglist();

	/* 获取 LED_IP */
	memset(gledip,0,sizeof(gledip));
	get_ledIP(TABNO_PARKINFO, gledip);

	/* 接收消息和接收文件服务 */
	getfilesvr_init(&SysCfg.Loc.FilePort);

	getmsgsvr_init(&SysCfg.Loc.MsgPort);
	/* 建立工作线程 */
	thre_init();
	
	for(;;)
	{
		sleep(1);
		timecnt++;
            char buf[32];
		memset(buf,0,sizeof(buf));
            int m=0;
            printf("-------------------------00000-----------:%d\r\n",m);
            m=full_remainspacefromtab();
            printf("-----------------------------1111111-------:%d\r\n",m);
            if(m >=0)
            {
                 
                 if(m >= SysCfg.SpaceFull_RealRmn) m = m -SysCfg.SpaceFull_RealRmn;
                 else m=0;
                 reminspace = m;
    		    sprintf(buf,"%d",m);
            
                 //sprintf(buf,"%d",spacrmn);	
    		//gledip;
    		//setLED_num("210.39.2.142",5005,buf);
    		setLED_num(gledip,5005,buf);
            }


        
		if(timecnt > 10)
		{
			timecnt=0;
		}
		switch(timecnt)
		{
			case 10:
			{
				keep_Space(USR_PATH_HIS_LOG,MAX_SPACE_LOG_HIS,"*");
				keep_Space(USR_PATH_INT_LOG,MAX_SPACE_LOG_INT,"*");
                
                    	if(rdtxt_instab(TABNO_PARKINFO) == 0) remove("/opt/euds/tmp/parkinfo.txt");
                    	if(rdtxt_instab(TABNO_AREAINFO) == 0) remove("/opt/euds/tmp/areainfo.txt");
                        
				break;
			}
			case 9:
			{
				performance_freespace();	
				break;
			}
			case 8:
			{
				cutlog2hisdir();				
				break;
			}
			case 7:
			{				
				cpu=0;
				mem=0;
				get_cpumen_info(&cpu,&mem);
				writelog(ZLOG_SYS,"cpu使用率是:%d%%,内存使用率是:%d%%",cpu,mem);
				if((cpu > MAX_ALLOW_CPU_USR)||(mem > MAX_ALLOW_MEM_USR))
				{
					writelog(ZLOG_SYS,"系统资源使用率偏高，该平台不适合存在系统问题，请排查，系统即将重启");
					sleep(1);
					reboot(RB_AUTOBOOT);
				}		
				break;
			}

			case 5:
			{				
				if(test_thres() != 0)
				{
					writelog(ZLOG_SYS,"线程异常退出，系统即将重启");
					sleep(1);
					reboot(RB_AUTOBOOT);			
				}	
				break;
			}
			default:
			{
				break;
			}
		}
	}
	writelog(ZLOG_ERR,"管理线程异常退出");
	return -1;
}

