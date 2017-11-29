#ifndef __MONITOR_H__
#define __MONITOR_H__

/* Ӳ�̿ռ���  */
#define MAX_SPACE_LOG_INT 30  //�����־����洢�ռ䣬��λ��M
#define MAX_SPACE_LOG_HIS 30

#define MAX_GET_CPU_TIME 10  //���λ�ȡCPU�ļ��ʱ��(s)

#define USR_PATH_HIS_LOG  "/opt/euds/log/history/"
#define USR_PATH_INT_LOG  "/opt/euds/log/intraday/"

#define USR_PATH_TPM  "/opt/euds/tmp/"


#define FILE_TYPE_LOG "*.txt"
#define FILE_TYPE_IMG "*.jpg"


extern int clrfile(const char *usrdir,char *filetype);
extern void get_cpumen_info(int *cpuuse_per,int *memuse_per);
extern void keep_Space(const char *stordir ,int maxspace,const char *filetype);
extern int cutlog2hisdir(void);


#endif

