#ifndef __MSGPROC_H__
#define __MSGPROC_H__

#include "gwlist.h"
#include "msglist.h"

#define UPL_TOTAL_CMD 0xF2
#define UPL_DETAIL_CMD 0xF3

extern int crtmsg(gw_list_s* glist,msg_list_s* mlist,
				int *spacttl,int *spacrmn,int *incnt_incar,int *abn);
extern int msg2mng(msg_list_s* mlist,char *mngIP, int mngPort);

#endif





