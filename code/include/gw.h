#ifndef __GW_H__
#define __GW_H__

#define SET_GW_INFO 0x0001A100 	/* 设置网关 */
#define GET_GW_RES 0x0001A200	/* 获取车位结果 */

#define ENABLE 1
#define DISENABLE 0




//extern int getinfo_gw(char *ip, unsigned int port, unsigned int cmdID,void *res);
//extern int setinfo_gw(gw_node_s *n, unsigned int cmdID);
extern int chkset_gwlist(gw_list_s *list);
extern int get_spaceInfo2list(gw_list_s *list);
#endif


