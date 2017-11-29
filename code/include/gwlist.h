#ifndef __GWLIST_H__
#define __GWLIST_H__

#define MAX_GWLIST_LEN 20				/* ��������� */
#define MAX_GWSPACE_LEN 60				/* һ�����ش�����೵λ�� */
#define MAX_ERR_RESET_TIME 3			/* ������ô����������ô��� */

#define GW_COMMU_ERR 0x01
#define DETOR_COMMU_ERR 0x01

typedef struct 
{
	char SpaceNO[8];
	u8 SpaceID;
	u8 InCnt;		/* �Ƿ������� */
	u8 InUse;		/* �Ƿ����ã�Ϊ1���ã�0���� */
	u8 UseLight;	/* �ƹ����ñ�־��1Ϊ���ã�0Ϊ���� */
	u8 BlueLight;	/* ��ɫָʾ�ƣ�1Ϊʹ����ɫָʾ�ƣ�0Ϊʹ�ú���ָʾ�� */
	u8 LastStat;
	u8 NowStat;
	u8 DevErr;		/* �豸״̬ */
}spaceinfo_s;


typedef struct gw_node_
{
	u8 gwID;
	char gwIP[32];
	u32 gwPort;
	u8 DevErr;		/* �豸����״̬ */
	u8 AllSpaceDevErr;/* ֻҪ�������һ��û�����óɹ�����ô��λ���ᱻ��λ */
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




