#ifndef __GWLIST_H__
#define __GWLIST_H__

#define MAX_GWLIST_LEN 20				/* 最多网关数 */
#define MAX_GWSPACE_LEN 60				/* 一个网关带的最多车位数 */
#define MAX_ERR_RESET_TIME 3			/* 最大设置错误重新设置次数 */

#define GW_COMMU_ERR 0x01
#define DETOR_COMMU_ERR 0x01

typedef struct 
{
	char SpaceNO[8];
	u8 SpaceID;
	u8 InCnt;		/* 是否参与计数 */
	u8 InUse;		/* 是否在用，为1在用，0禁用 */
	u8 UseLight;	/* 灯光启用标志，1为启用，0为禁用 */
	u8 BlueLight;	/* 蓝色指示灯，1为使用蓝色指示灯，0为使用红绿指示灯 */
	u8 LastStat;
	u8 NowStat;
	u8 DevErr;		/* 设备状态 */
}spaceinfo_s;


typedef struct gw_node_
{
	u8 gwID;
	char gwIP[32];
	u32 gwPort;
	u8 DevErr;		/* 设备连接状态 */
	u8 AllSpaceDevErr;/* 只要检测器有一个没有设置成功，那么该位都会被置位 */
	u8 unConnTc;
	spaceinfo_s SpaceInfo[MAX_GWSPACE_LEN];
	struct gw_node_ *Next;
}gw_node_s;

typedef struct
{
	gw_node_s *Head;
	gw_node_s *Tail;
	u8 Len;
	int ParkId;
	int GarageId; 
	pthread_mutex_t Mtx;
	pthread_cond_t Cond;
}gw_list_s;


extern gw_list_s* init_gwlist(void);
extern int ins_gwlist(gw_list_s *q,int gwId);


#endif




