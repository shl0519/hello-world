#include "include.h"

typedef struct          //����һ��cpu occupy�Ľṹ��
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
* ������  : get_memoccupy
* ����	  : ��ȡ�ڴ�ʹ����
* ����	  : ��
* ���	  : �ڴ�ʹ����
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
* ������  : cal_cpuoccupy
* ����	  : ����CPUʹ����
* ����	  : *o:��һ�λ�ȡCPU��Ϣ
			*n:�ڶ��λ�ȡCPU��Ϣ
* ���	  : CPUʹ����
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
* ������  : get_cpuoccupy
* ����	  : ��ȡCPU��Ϣ
* ����	  : ��			
* ���	  : *cpust��cpu��Ϣ�ṹ��
* ����	  : 0:����
			-1:����
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
* ������  : get_cpumen_info
* ����	  : ��ȡCPU�ڴ�ʹ����
* ����	  : ��
* ���	  : *cpuuse_per,cpuʹ����
		  : *memuse_per���ڴ�ʹ����
* ����	  : 0:����
			-1:����
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
* ������  : get_sh_res
* ����	  : ��ȡshell���
* ����	  : glist,���ض���
		  : mlist,��Ϣ����
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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
	/* �����fork����pipe������ôfile=null */
	if(file == NULL)
	{
		writelog(ZLOG_SYS, "ִ���������:%s",cmd);
		return err;
	}
	/* ���shִ��û�������ô�ļ�����Ϊ�� */
    else if(fgets(res, len, file) != NULL) 
	{
		err=0;
    }
    pclose(file);
	return err;
}
/*******************************************************************************
* ������  : keep_Space
* ����	  : ����Ŀ¼��ռ�õĿռ䣬���������������
* ����	  : stordir,Ŀ¼
		  : maxspace,�������ռ�
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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
		writelog(ZLOG_SYS,"ִ���������:%s",buf);
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
		writelog(ZLOG_SYS,"Ŀ¼�ռ�:%dM,���ھ���ֵ��ɾ���ļ�����:%s",space,buf);
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
	if((usrdir == NULL)||(filetype == NULL)) 	/*�ж϶�ǰ�Ƿ�Ϊ�գ�����Ǿͽ���*/
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
//		closedir(dir);//_by_pbh_2015-01-26 ע��closedir�رտյľ��
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
			writelog(ZLOG_SYS,"�����ļ�:%s",ptmp);
		}
	}
	closedir(dir);
	return 0;
}

/*----------------------------------------------------
* ������  : cutlog2hisdir
* ����	  : ��־������������������־��������ʷ��־
* ����    :	@*pathfile:��Ŀ¼Ҳ���Բ���Ŀ¼
			@socketfd:�Ѿ����ӵľ��
			
* ���	  : ��
* ����	  : 0: ����
			-1:����
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


