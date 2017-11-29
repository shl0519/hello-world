
#include "zlog_usr.h"

static zlog_category_t *fd_zlog = NULL;

int ZlogInit(void)
{
	int rc = -1;
	rc = zlog_init(PATH_LOGCONF);
	if( rc != 0 )
	{
		printf("zlog inition faile\r\n");
		return -1;
	}

	fd_zlog = zlog_get_category(LOG_CLASS);
	if( fd_zlog == NULL )
	{
		printf("zlog Get cat fail\r\n");
		zlog_fini();
		return -2;
	}
	writelog(ZLOG_SYS,"超声波采集软件停电或关机重启程序.\r\n");
	writelog(ZLOG_GW,"超声波采集软件停电或关机重启程序.\r\n");
	writelog(ZLOG_MNG,"超声波采集软件停电或关机重启程序.\r\n");
	writelog(ZLOG_SQL,"超声波采集软件停电或关机重启程序.\r\n");
	writelog(ZLOG_ERR,"超声波采集软件停电或关机重启程序.\r\n");
	writelog(ZLOG_LANE,"超声波采集软件停电或关机重启程序.\r\n");
	return 0;
}

void ZlogDele(void)
{
	zlog_fini();
}

int ZlogWrite(char *buf, loglevel flag)
{
	switch(flag)
	{
		case ZLOG_SYS:
			zlog_sys(fd_zlog, buf);
			break;
		case ZLOG_GW:
			zlog_gw(fd_zlog, buf);
			break;
		case ZLOG_MNG:
			zlog_mng(fd_zlog, buf);
			break;
		case ZLOG_SQL:
			zlog_sql(fd_zlog, buf);
			break;
		case ZLOG_ERR:
			zlog_err(fd_zlog, buf);
			break;
		case ZLOG_LANE:
			zlog_lane(fd_zlog, buf);
			break;
		default:
			break;
	}
	zlog_debug(fd_zlog, buf);
	return 0;
}


/*=====================================================================
 * 函数名：copyFile
 * 功能：拷贝文件函数
 * 参数：
 *		srcfile：源文件名（带路径）
 *		targetfile：目标文件名（带路径）
 * 返回值：
 *		0: 拷贝成功
 *		-1：拷贝失败
 * author:
 ======================================================================*/
int copyFile(char *srcfile, char *targetfile)
{
	FILE *fpR, *fpW;
	char buffer[2048];
	int lenR, lenW;
	if ((fpR = fopen(srcfile, "r")) == NULL)
	{
		writelog(ZLOG_SYS,"Error:The file '%s' can not be opened!",srcfile);
//		fclose(fpR);//_by_pbh_2015-01-26 注释fclose关闭空的句柄
		return -1;
	}
	if ((fpW = fopen(targetfile, "a")) == NULL)//以追加的方式打开_by_pbh_2014-11-17
	{
		writelog(ZLOG_SYS,"Error:The file '%s' can not be opened!",targetfile);
//		fclose(fpW);//_by_pbh_2015-01-26 注释fclose关闭空的句柄
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	while ((lenR = fread(buffer, 1, 2048, fpR)) > 0)
	{
		if ((lenW = fwrite(buffer, 1, lenR, fpW)) != lenR)
		{
			writelog(ZLOG_SYS,"Error:Write to file '%s' failed!",targetfile);
			fclose(fpR);
			fclose(fpW);
			return -1;
		}
		memset(buffer, 0, sizeof(buffer));
	}

	fclose(fpR);
	fclose(fpW);

	writelog(ZLOG_SYS,"%s copy to %s",srcfile,targetfile);//_by_pbh_2015-03-06
	return 0;
}

/*=====================================================================
_by_pbh_数据库:每天0点调用vacuum命令释放磁盘空间，改写cpylog2hisdir函数返回值_20150623
 * 函数名：cpylog2hisdir
 * 功能：查找目录下有没有.txt文件
 * 参数：
 *		srcfile：源文件名（带路径）
 *		targetfile：目标文件名（带路径）
 *		logtime:日志类型，是历史记录还是当天记录
 * 返回值：
 *		2:  0点时刻，拷贝日志文件成功，logtime为HISTORY
 *		1:  下发指令，拷贝日志文件成功，logtime为INTRADAY
 *		0： 没有满足拷贝日志文件的条件
 *		-1：执行失败
 * author:shl
 * date:2014-11-06
 ======================================================================*/
int cpylog2hisdir(const char* srcdir,const char *targetdir, log_time logtime)
{
	if((srcdir == NULL)||(targetdir == NULL)) 	/* 参数合法性判断 */
	{
		writelog(ZLOG_SYS, "[cpylog2hisdir] Invalid argument!");
		return -1;
	}

	DIR * dir;
	struct dirent * ptr;

	dir = opendir(srcdir);
	if(dir == NULL)
	{
		writelog(ZLOG_SYS,"%s open dir error!",srcdir);
		return -1;
	}
	
	char datebuf[16];
	struct tm  *timeinfo;
	struct	timeval t;
	gettimeofday(&t,NULL);
	timeinfo = localtime(&(t.tv_sec));
	memset(datebuf,0,sizeof(datebuf));
	strftime (datebuf,16,"%Y-%m-%d",timeinfo);

	while((ptr = readdir(dir)) != NULL)
	{
		char *strFileType;
		char *strDate;
		strFileType = strstr(ptr->d_name,".txt");	/* 包含.txt子串 */
		strDate = strstr(ptr->d_name,datebuf);		/* 包含当前日期的 */

		if((strFileType != NULL)
			&&(((strDate != NULL) && (logtime == INTRADAY))
			||((strDate == NULL) && (logtime == HISTORY))))
		{
			char bufs[256];
			char buft[256];
			memset(bufs,0,sizeof(bufs));
			memset(buft,0,sizeof(buft));
			strcat(bufs,srcdir);
			strcat(bufs,ptr->d_name);
			strcat(buft,targetdir);
			strcat(buft,ptr->d_name);
			if(copyFile(bufs,buft) != 0)
			{
				closedir(dir);
				writelog(ZLOG_SYS,"Error: [cpylog2hisdir] copy file error!");
				return -1;
			}
			if(logtime == HISTORY)/* 在当天日志目录把历史日志删除 */
			{
				delfile(bufs);
			}
		}
	}
	closedir(dir);
	return SUCCESS;
}




