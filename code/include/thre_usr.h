#ifndef __THRE_USR_H__
#define __THRE_USR_H__

#include "gwlist.h"
#include "msglist.h"

#define MAX_THRE_TOTAL 4
#define CLT_THRE 0
#define PROC_THRE 1
#define MSG_THRE 2
#define GPIO_THRE 3

extern gw_list_s* gwlist;
extern msg_list_s* msglist;
extern int thre_init(void);
extern int test_thres(void);
extern int reminspace;

#endif





