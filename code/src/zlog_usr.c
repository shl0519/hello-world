
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
	writelog(ZLOG_SYS,"�������ɼ����ͣ���ػ���������.\r\n");
	writelog(ZLOG_GW,"�������ɼ����ͣ���ػ���������.\r\n");
	writelog(ZLOG_MNG,"�������ɼ����ͣ���ػ���������.\r\n");
	writelog(ZLOG_SQL,"�������ɼ����ͣ���ػ���������.\r\n");
	writelog(ZLOG_ERR,"�������ɼ����ͣ���ػ���������.\r\n");
	writelog(ZLOG_LANE,"�������ɼ����ͣ���ػ���������.\r\n");
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
 * ��������copyFile
 * ���ܣ������ļ�����
 * ������
 *		srcfile��Դ�ļ�������·����
 *		targetfile��Ŀ���ļ�������·����
 * ����ֵ��
 *		0: �����ɹ�
 *		-1������ʧ��
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
//		fclose(fpR);//_by_pbh_2015-01-26 ע��fclose�رտյľ��
		return -1;
	}
	if ((fpW = fopen(targetfile, "a")) == NULL)//��׷�ӵķ�ʽ��_by_pbh_2014-11-17
	{
		writelog(ZLOG_SYS,"Error:The file '%s' can not be opened!",targetfile);
//		fclose(fpW);//_by_pbh_2015-01-26 ע��fclose�رտյľ��
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
_by_pbh_���ݿ�:ÿ��0�����vacuum�����ͷŴ��̿ռ䣬��дcpylog2hisdir��������ֵ_20150623
 * ��������cpylog2hisdir
 * ���ܣ�����Ŀ¼����û��.txt�ļ�
 * ������
 *		srcfile��Դ�ļ�������·����
 *		targetfile��Ŀ���ļ�������·����
 *		logtime:��־���ͣ�����ʷ��¼���ǵ����¼
 * ����ֵ��
 *		2:  0��ʱ�̣�������־�ļ��ɹ���logtimeΪHISTORY
 *		1:  �·�ָ�������־�ļ��ɹ���logtimeΪINTRADAY
 *		0�� û�����㿽����־�ļ�������
 *		-1��ִ��ʧ��
 * author:shl
 * date:2014-11-06
 ======================================================================*/
int cpylog2hisdir(const char* srcdir,const char *targetdir, log_time logtime)
{
	if((srcdir == NULL)||(targetdir == NULL)) 	/* �����Ϸ����ж� */
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
		strFileType = strstr(ptr->d_name,".txt");	/* ����.txt�Ӵ� */
		strDate = strstr(ptr->d_name,datebuf);		/* ������ǰ���ڵ� */

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
			if(logtime == HISTORY)/* �ڵ�����־Ŀ¼����ʷ��־ɾ�� */
			{
				delfile(bufs);
			}
		}
	}
	closedir(dir);
	return SUCCESS;
}




