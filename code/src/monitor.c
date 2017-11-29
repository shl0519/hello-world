#include "include.h"

typedef struct          //定义一个cpu occupy的结构体
{
	char name[20];   
	unsigned int user; 
	unsigned int nice; 
	unsigned int system;
	unsigned int idle;
}cpu_occupy_s;

typedef struct
{
	char totalname[32]; 
	unsigned long total; 
	char freename[32];
	unsigned long free;                       
}mem_occupy_s;



/*******************************************************************************
* 函数名  : get_memoccupy
* 功能	  : 获取内存使用率
* 输入	  : 无
* 输出	  : 内存使用率
*******************************************************************************/
int get_memoccupy (void)
{
    FILE *fd;                      
    char buff[256];   
    mem_occupy_s mem;
                                                                                                              
    fd = fopen ("/proc/meminfo", "r"); 
      
    fgets (buff, sizeof(buff), fd); 
	sscanf (buff, "%s %u", mem.totalname, &mem.total); 
    fgets (buff, sizeof(buff), fd); 
	sscanf (buff, "%s %u", mem.freename, &mem.free); 
	
    fclose(fd);
	
    return(100 - 100 * mem.free / mem.total);
}
/*******************************************************************************
* 函数名  : cal_cpuoccupy
* 功能	  : 计算CPU使用率
* 输入	  : *o:第一次获取CPU信息
			*n:第二次获取CPU信息
* 输出	  : CPU使用率
*******************************************************************************/
int cal_cpuoccupy (cpu_occupy_s *o, cpu_occupy_s *n) 
{   
    unsigned long od, nd;    
    unsigned long id, sd;
    int cpu_use = 0;   
    
    od = (unsigned long) (o->user + o->nice + o->system +o->idle);
    nd = (unsigned long) (n->user + n->nice + n->system +n->idle);
      
    id = (unsigned long) (n->user - o->user); 
    sd = (unsigned long) (n->system - o->system);
    if((nd-od) != 0)
    cpu_use = (int)((sd+id)*100)/(nd-od);//10000
    else cpu_use = 0;
    //printf("cpu: %u/n",cpu_use);
    return cpu_use;
}
/*******************************************************************************
* 函数名  : get_cpuoccupy
* 功能	  : 获取CPU信息
* 输入	  : 无			
* 输出	  : *cpust，cpu信息结构体
* 返回	  : 0:正常
			-1:错误
*******************************************************************************/
void get_cpuoccupy(cpu_occupy_s *cpust) 
{   
    FILE *fd;                    
    char buff[256]; 
    cpu_occupy_s *cpu_occupy;
    cpu_occupy=cpust;
                                                                                                               
    fd = fopen ("/proc/stat", "r"); 
    fgets (buff, sizeof(buff), fd);
    
    sscanf (buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);
    
    fclose(fd);     
}
/*******************************************************************************
* 函数名  : get_cpumen_info
* 功能	  : 获取CPU内存使用率
* 输入	  : 无
* 输出	  : *cpuuse_per,cpu使用率
		  : *memuse_per，内存使用率
* 返回	  : 0:正常
			-1:错误
*******************************************************************************/
void get_cpumen_info(int *cpuuse_per,int *memuse_per)
{
    cpu_occupy_s cpu_stat1;
    cpu_occupy_s cpu_stat2;

    int cpu;
	int mem;
    
    mem=get_memoccupy ();
	*memuse_per = mem;
    
    get_cpuoccupy((cpu_occupy_s *)&cpu_stat1);
    sleep(MAX_GET_CPU_TIME);
    
    get_cpuoccupy((cpu_occupy_s *)&cpu_stat2);
    
    cpu = cal_cpuoccupy ((cpu_occupy_s *)&cpu_stat1, (cpu_occupy_s *)&cpu_stat2);
	*cpuuse_per = cpu;

} 



/*******************************************************************************
* 函数名  : get_sh_res
* 功能	  : 获取shell结果
* 输入	  : glist,网关队列
		  : mlist,消息队列
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int get_sh_res(char *cmd, char *res,int len) 
{
	if((cmd == NULL)||(res == NULL))
	{
		writelog(ZLOG_SYS, "get_sh_res Invalid argument!");
		return -1;		
	}
	
	FILE *file;
	int err=-1;
	file = popen(cmd, "r");
	/* 如果是fork或者pipe错误，那么file=null */
	if(file == NULL)
	{
		writelog(ZLOG_SYS, "执行命令错误:%s",cmd);
		return err;
	}
	/* 如果sh执行没结果，那么文件内容为空 */
    else if(fgets(res, len, file) != NULL) 
	{
		err=0;
    }
    pclose(file);
	return err;
}
/*******************************************************************************
* 函数名  : keep_Space
* 功能	  : 测试目录所占用的空间，如果超出，则清理
* 输入	  : stordir,目录
		  : maxspace,最大允许空间
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
void keep_Space(const char *stordir ,int maxspace,const char *filetype) 
{
	char buf[128];
	char res[64];
	int space;
	
	memset(buf,0,sizeof(buf));
	strcpy(buf,"du -sk ");
	strcat(buf,stordir);
	strcat(buf," | cut -f 1");
	if(get_sh_res(buf,res,64) != 0)
	{
		writelog(ZLOG_SYS,"执行命令错误:%s",buf);
		return;
	}
	space=atoi(res)/1000;
	if(space >= maxspace)
	{
		memset(buf,0,sizeof(buf));
		strcpy(buf,"ls -rt ");
		strcat(buf,stordir);
		strcat(buf,filetype);
		strcat(buf," | head -2 | xargs rm -rf");
		printf("----------------:%s\r\n",buf);
		//strcpy(buf,"ls -rt /mnt/data/log/history/*.txt | head -2 | xargs rm -rf");
		system(buf);
		writelog(ZLOG_SYS,"目录空间:%dM,大于警戒值，删除文件命令:%s",space,buf);
	}

}

/*****************************************************************************
* delDir_TmpFile
* DESCRIPTION : see the dir ,wherther it have the finename
* @dirstring  :
* @filename   : doorlane
* @Return	  : if ok return 0,error return -1;
* ----
******************************************************************************/
int clrfile(const char *usrdir,char *filetype)
{
	if((usrdir == NULL)||(filetype == NULL)) 	/*判断对前是否为空，如果是就结束*/
	{
		writelog(ZLOG_SYS, "clrfile Invalid argument!");
		return -1;
	}

	DIR * dir;
	struct dirent * ptr;
	char * str1;
	
	dir = opendir(usrdir);
	if(dir == NULL)
	{
		writelog(ZLOG_SYS,"%s open error!",usrdir);
//		closedir(dir);//_by_pbh_2015-01-26 注释closedir关闭空的句柄
		return -1;
	}

	while((ptr = readdir(dir)) != NULL)
	{
		str1 = strstr(ptr->d_name,filetype);
		if(str1 != NULL )
		{
			char ptmp[256];
			memset(ptmp,0,sizeof(ptmp));
			strcat(ptmp,usrdir);
			strcat(ptmp,ptr->d_name);
			remove(ptmp);
			writelog(ZLOG_SYS,"清理文件:%s",ptmp);
		}
	}
	closedir(dir);
	return 0;
}

/*----------------------------------------------------
* 函数名  : cutlog2hisdir
* 功能	  : 日志处理函数，包含当天日志拷贝到历史日志
* 输入    :	@*pathfile:带目录也可以不带目录
			@socketfd:已经连接的句柄
			
* 输出	  : 无
* 返回	  : 0: 正常
			-1:错误
----------------------------------------------------*/
int cutlog2hisdir(void)
{
	if(cpylog2hisdir(USR_PATH_INT_LOG,USR_PATH_HIS_LOG,HISTORY) != SUCCESS)
	{
		writelog(ZLOG_SYS, "ERROR:copy history log intraday to history dir fail!!");
		writelog(ZLOG_SYS, "ERROR:it will be copy again next 10 seconds!");
		return -1;
	}
	return SUCCESS;
}


