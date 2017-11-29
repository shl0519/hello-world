#ifndef __SQLITE3_USR_H__
#define __SQLITE3_USR_H__

#define PATH_DATABASE "/opt/euds/base/euds.db"
#define PATH_TMP "/opt/euds/tmp/"

#define TAB_TOTAL 2 

#define TABNO_PARKINFO		0	/* 0 为配置文件，其他的一一对应 */
#define TABNO_AREAINFO		1


extern const char m2l_txtname[TAB_TOTAL][32];

/*
#define INUSE 1
#define NOUSE 0
*/

extern int database_init(void);
extern int rdtxt_instab(int tabno);
extern int get_gwID2list(int fileno, gw_list_s *list);
extern int full_gwlistinfo(gw_list_s *list);
extern int update_spaceinfo2db(gw_list_s *list);
extern int full_gwlistheadinfo(int fileno,gw_list_s *q);
extern int get_ledIP(int fileno, char *ip);
extern int performance_freespace(void);
extern int full_remainspacefromtab(void);
#endif

