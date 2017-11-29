/************************************************************
#	文件名: 	zlog.h
#	文件内容:
#
#	修改时间1:
#	修改内容1:
#
#	编写者:	daipj
#	完成时间:
************************************************************/
#ifndef __ZLOG_USR_H__
#define __ZLOG_USR_H__

#include "include.h"

#define PATH_LOGCONF "/opt/euds/etc/euds_zlog.conf"
#define LOG_CLASS "euds"

/* 定义日志使用等级*/
typedef enum
{
	ZLOG_SYS = 41,
	ZLOG_GW = 42, /* 网关通信日志 */
	ZLOG_MNG = 43,/* 和管理机通信 */
	ZLOG_SQL = 44,
	ZLOG_ERR = 45,
	ZLOG_LANE = 46,
}loglevel;

typedef  enum {		//_by_shl_日志上传时间类型_20141106
	INTRADAY=0x00,
	HISTORY=0x01,
}log_time;

#define zlog_sys(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_SYS, format, ##args)

#define zlog_gw(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_GW, format, ##args)

#define zlog_mng(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_MNG, format, ##args)

#define zlog_sql(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_SQL, format, ##args)

#define zlog_err(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_ERR, format, ##args)

#define zlog_lane(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LANE, format, ##args)


#define writelog(loglevel,format,...) \
	do{ \
		char zlogbuf[1024] = {0};\
		char zlogdebugbuf[50] = {0};\
		char *zlogp = __FILE__;\
		char *zlogfile = strrchr(zlogp, '/');\
		if( zlogfile != NULL ) zlogp = zlogfile + 1;\
		sprintf(zlogdebugbuf, "[%.13s:%04d] " , zlogp, __LINE__);\
		sprintf(zlogbuf, "%-21s" format"\r", zlogdebugbuf, ##__VA_ARGS__ );\
		ZlogWrite(zlogbuf, loglevel);\
	}while(0)

extern int ZlogInit(void);
extern void ZlogDele(void);
extern int ZlogWrite(char *buf, loglevel flag);
extern int copyFile(char *srcfile, char *targetfile);//_by_shl_2014-11-06
extern int cpylog2hisdir(const char* srcdir,const char *targetdir, log_time logtime);//_by_shl_2014-11-06


#endif



