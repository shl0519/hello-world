#ifndef __SYSCFG_H__
#define __SYSCFG_H__

#define PATH_SYS_CONFIG "/opt/euds/etc/euds.conf"

typedef struct
{
	char IP[20];
	int MsgPort;
	int FilePort;
	char GateWay[20];
}local_commu_s;


typedef struct
{
	char IP[20];
	int MsgPort;
	int FilePort;
}mng_commu_s;


typedef struct
{
	local_commu_s Loc;
	mng_commu_s Mng;
	int Modul_3G;
	gpiname SnapCoil;
	gponame RailUp;
	int SpaceFull_RealRmn;
	int RailupPulseWidth;
    	local_commu_s UsrCommn;
}sys_config_s;

extern sys_config_s SysCfg;
extern int delfile(char* pathfile);
extern void rd_syscfg(void);

#endif

