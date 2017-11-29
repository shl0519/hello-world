#include "include.h"

/* sqlite3建立表，命名不能包含点号 */
/* 如ar.tab，命令执行的时候会是错误的 */
/* 表名放置顺序要与没m2l.c里面的一致 */

const char m2l_txtname[TAB_TOTAL][32]=
{
	"parkinfo.txt",
	"areainfo.txt",
};

const char tab_name[TAB_TOTAL][32]=
{
	"parkinfo_tab",
	"areainfo_tab",
};
const char tmptab_name[TAB_TOTAL][32]=
{
	"parkinfo_tmp_tab",
	"areainfo_tmp_tab",
};

const char tab_struct[TAB_TOTAL][256] =
{
	//"parkinfo_tab",
	"(ParkId int,\
GarageId smallint,\
GarageIp character varchar(32),\
AreaID smallint,\
AreaLedId smallint,\
AreaLedIp character varchar(32));",

	//"areainfo_tab",
	"(AreaId smallint,\
GwId smallint,\
GwIp character varchar(32),\
GwPort int,\
SpaceNo character varchar(8),\
SpaceId smallint,\
InCnt smallint,\
InUse smallint,\
UseLight smallint,\
BlueLight smallint,\
State smallint,\
DevErr smallint,\
primary key(GwId,SpaceId,SpaceNo));",

};

const char tab_struct_nokey[TAB_TOTAL][256] =
{
	//"parkinfo_tab",
	"(ParkId int,\
GarageId smallint,\
GarageIp character varchar(32),\
AreaID smallint,\
AreaLedId smallint,\
AreaLedIp character varchar(32));",

	//"areainfo_tab",
	"(AreaId smallint,\
GwId smallint,\
GwIp character varchar(32),\
GwPort int,\
SpaceNo character varchar(8),\
SpaceId smallint,\
InCnt smallint,\
InUse smallint,\
UseLight smallint,\
BlueLight smallint,\
State smallint,\
DevErr smallint);",

};


const char tabNO_ColumnTotal[TAB_TOTAL]={6,12};
const char *parkinfo_coloum[]=
{
	"ParkId", 
	"GarageId", 
	"GarageIp", 
	"AreaID", 
	"AreaLedId", 
	"AreaLedIp"
};
const char *areainfo_coloum[]=
{
	"AreaId",
	"GwId", 
	"GwIp",
	"GwPort",
	"SpaceNo", 
	"SpaceId",
	"InCnt",
	"InUse",
	"UseLight",
	"BlueLight",
	"State",
	"DevErr"
};


const char **tabcoloumname[TAB_TOTAL]=
{
	parkinfo_coloum,
	areainfo_coloum
};


static sqlite3 *fd;

int checkpkey(char *des, char *src);
int deletetab_partrec(int fileno);




/*****************************************************************************
* DropTab_Cmd,RenameTab_Cmd,CreattTab_Cmd,ChkTab_Cmd
* DESCRIPTION: 这几个函数作用是生成SQLITE命令，通过组合
* @Param  : 几个
* @Return : none
* ----
******************************************************************************/
void droptab_cmd(const char *tabname,char *buf)
{
	strcat((char *)buf,"drop table ");
	strcat((char *)buf,tabname);
}

void renametab_cmd(const char *old_tabname,const char *new_tabname,char *buf)
{
	strcat((char *)buf,"alter table ");
	strcat((char *)buf,old_tabname);
	strcat((char *)buf," rename to ");
	strcat((char *)buf,new_tabname);
}

void creatttab_cmd(const char *tabname,const char *tabstruct, char *buf)
{
	strcat((char *)buf,"create table ");
	strcat((char *)buf,tabname);
	strcat((char *)buf,(char *)tabstruct);
	writelog(ZLOG_SQL,"%s",buf);
}

void chktab_cmd(const char *tabname,char *buf)
{
	strcat((char *)buf,"select count(*) as cnt from sqlite_master where type='table' and name='");
	strcat((char *)buf,tabname);
	strcat((char *)buf,"';");
	writelog(ZLOG_SQL,"%s",buf);
}
void chkcolumn_cmd(const char *tabname,const char *column, char *buf)
{
	strcat((char *)buf,"select count(1) from sqlite_master where tbl_name='");
	strcat((char *)buf,tabname);
	strcat((char *)buf,"' and sql like '%");
	writelog(ZLOG_SQL,"%s",buf);//_by_pbh_2014-12-08 zlog日志内容有%，导致M3352段错误
	strcat((char *)buf,column);
	strcat((char *)buf,"%';");
}
void createcolumn_cmd(const char *tabname,const char *column,const char *datetype, char *buf)
{
	strcat((char *)buf,"alter table ");
	strcat((char *)buf,tabname);
	strcat((char *)buf," add ");
	strcat((char *)buf,column);
	strcat((char *)buf," ");
	strcat((char *)buf,datetype);
	strcat((char *)buf,";");
	writelog(ZLOG_SQL,"%s",buf);
}

//增加创建数据库表索引_by_pbh_2014-08-27
void createindex_cmd(const char *tabname,const	char *indexfield, char *buf)
{
	strcat((char *)buf,"create index ");
	strcat((char *)buf,tabname);
	strcat((char *)buf,"_");
	strcat((char *)buf,(char *)indexfield);
	strcat((char *)buf," on ");
	strcat((char *)buf,tabname);
	strcat((char *)buf,"(");
	strcat((char *)buf,(char *)indexfield);
	strcat((char *)buf,");");
	writelog(ZLOG_SQL,"%s",buf);
}
//增加删除数据库表索引_by_pbh_2014-08-27
void dropindex_cmd(const char *tabname,const  char *indexfield, char *buf)
{
	strcat((char *)buf,"drop index ");
	strcat((char *)buf,tabname);
	strcat((char *)buf,"_");
	strcat((char *)buf,(char *)indexfield);
	strcat((char *)buf,";");
	writelog(ZLOG_SQL,"%s",buf);
}

void updatecolumn_cmd(const char *tabname,const char *column,const	char value, char *buf)
{
	char mdata[10];
	memset(mdata,0,sizeof(mdata));
	strcat((char *)buf,"update ");
	strcat((char *)buf,tabname);
	strcat((char *)buf," set ");
	strcat((char *)buf,column);
	strcat((char *)buf,"=");
	sprintf(mdata, "%d",value);
	strcat((char *)buf,mdata);
	strcat((char *)buf,";");
	writelog(ZLOG_SQL,"%s",buf);
}

/*****************************************************************************
* Exec_SqlCmd
* DESCRIPTION: excute the two sql command,
* @Param  : two
* @Return : if excute OK,return 0,else excute faile return -1
* ----
******************************************************************************/
int Exec_SqlCmd(char *sqlcmd)
{
	char *errmsg;
	int res=-1;
	res = sqlite3_exec(fd,sqlcmd,NULL,NULL,&errmsg);//执行不需要返回数据的查询
	if ((res != SQLITE_OK)&&(res != SQLITE_ERROR))
	{
		writelog(ZLOG_ERR,"failed exec!, err_code: %d, err_reason: %s", res, errmsg);
		sqlite3_free(errmsg);
		return -1;
	}
	sqlite3_free(errmsg);
	return 0;
}

/*****************************************************************************
* update_TabRec
* DESCRIPTION: insert to the table access records table or park_num tab
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int update_TabRec(const char *tabname,char *updateinfo,char *cds)
{
	char sqlcmd[256];
	memset(sqlcmd,0,sizeof(sqlcmd));
	sprintf(sqlcmd, "update %s set %s where %s",tabname,updateinfo,cds);
	
	writelog(ZLOG_SQL,"%s",sqlcmd);

	char *errmsg;
	int res=-1;
	res = sqlite3_exec(fd, sqlcmd, NULL, NULL, &errmsg);
	if (res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_free(errmsg);
		return -1;
	}
	sqlite3_free(errmsg);
	return 0;

}

/*****************************************************************************
* chkcreate_tab
* DESCRIPTION: check if the database and tab is exist,
* create the database,create all tables
* @Param  : three
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int chkcreate_tab(const char *tabname, const char *tabstruct)
{
	char buf[1024];

	char *errmsg;
	sqlite3_stmt *stmt;
	const char *tail;
	int res=-1;
	int ncols=-1;

	memset(buf,0,sizeof(buf));/* 生成检查命令 */
	chktab_cmd(tabname,buf);
	//printf("xxxxxxxx:%s",buf);//debug_shl_20140725
	res= sqlite3_prepare(fd, (char *)buf, (int)strlen((char *)buf), &stmt, &tail);

	if(res != SQLITE_OK)
	{
		writelog(ZLOG_ERR,"err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);
		return -1;
	}
	res=-1;
	res = sqlite3_step(stmt);

	ncols = sqlite3_column_count(stmt);
	if(ncols != 1)
	{
		writelog(ZLOG_ERR,"The database has two same name table,please check it");
		sqlite3_finalize(stmt);
		return -1;
	}

	if(res == SQLITE_ROW)//_by_pbh_2014-01-03数据库检查建表函数sqlite3_step返回值判断
	{
		if(strcmp((char *)sqlite3_column_text(stmt, 0),"1")!=0)
		{
			writelog(ZLOG_SQL,"Create table %s!",tabname);
			memset(buf,0,sizeof(buf));/* 生成建立表格命令 */
			creatttab_cmd(tabname,tabstruct,buf);
			res = sqlite3_exec(fd,(char *)buf,NULL,NULL,&errmsg);//执行不需要返回数据的查询
			if (res != SQLITE_OK)
			{
				writelog(ZLOG_ERR,"failed exec!, err_code: %d, err_reason: %s", res, errmsg);
				sqlite3_free(errmsg);
				sqlite3_finalize(stmt);
				return -1;
			}
			sqlite3_free(errmsg);
		}
	}
	sqlite3_finalize(stmt);
	return 0;

}

/*****************************************************************************
* InsLocDB_TmpTab
* DESCRIPTION : 把参数里头的记录插入到指定的临时表里面
* @db		  : 数据库连接句柄
* @tmp_tabname: 临时表名
* @record	  : 要插入的记录指针
* @Return	  : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int InsLocDB_TmpTab(int tabno, char *record)//_by_pbh_修改数据表文件下发功能_20150317
{
	char sqlbuf[512];
	char tmp[128];
	char *p_rec;
	char **p_col;
	int col_amount;
	p_rec = record;
	p_col = (char**)tabcoloumname[tabno];

	memset(sqlbuf,0,sizeof(sqlbuf));
	strcat(sqlbuf,"insert into ");
	strcat(sqlbuf,tmptab_name[tabno]);
	strcat(sqlbuf,"(");

	int i=0;
	int j=0;
	for(i=0; (*p_rec) != '\0'; i++)		/* 算出有多少个字段 */
	{
		if((*p_rec) == ';')
		{
			j++;
		}
		p_rec++;
	}
	if( j == 0 )
	{
		return 0;
	}

	for(i=0; (i<j)&&(i<tabNO_ColumnTotal[tabno]); i++)	/* 把字段名称写入SQL */
	{
		strcat(sqlbuf,*p_col);
		strcat(sqlbuf,",");
		p_col++;
	}
	col_amount = i;
	i=strlen(sqlbuf) - 1;				/* 找出最后一个逗号 */
	sqlbuf[i] = ')';					/* 替换成有括号 */
	p_rec = record;						/* 重新定位记录指针 */
	strcat(sqlbuf," values('");

	for(i=0; i<col_amount; i++)	/* 把字段名称写入SQL */
	{
		memset(tmp,0,sizeof(tmp));
		for(j=0; j<128; j++)
		{
			if((*p_rec) == ';')
			{
				break;
			}
			tmp[j] = *p_rec;
			p_rec++;
		}
		p_rec++; /* 指向下一个字符 */
		tmp[j]='\0';
		strcat(sqlbuf,tmp);
		if(i < (col_amount-1))
		{
			strcat(sqlbuf,"','");
		}
		else
		{
			strcat(sqlbuf,"')");
		}
	}

//	writelog(ZLOG_LEVEL_SQL,"%s", sqlbuf);
	return(Exec_SqlCmd(sqlbuf));
}

/*****************************************************************************
* updatetab_allrec
* DESCRIPTION : 把下发的数据库表文件，里头的记录插入到各个表里面，
* @txt_name   : 在临时目录下的接受的文本文件
* @tmp_tabname:在数据库里头，插入到各个表的临时文件，插入完成后，把原来旧的表删除
				  现在的表名更改为新的名字
* @tabname	  : 临时文件更改后的表明
* @Return	  : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int updatetab_allrec(int tabno)
{
	char sqlcmd[512];

	//旧表全表记录删除
	memset(sqlcmd,0,sizeof(sqlcmd));
	strcat(sqlcmd,"delete from ");
	strcat(sqlcmd,tab_name[tabno]);
	strcat(sqlcmd,";");

	//逐条记录拷贝
//	strcat(sqlcmd,"replace into ");
	strcat(sqlcmd,"insert into ");//_by_pbh_全表更新采用insert插入方式_2015-03-30
	strcat(sqlcmd,tab_name[tabno]);
	strcat(sqlcmd, "(");
	int i;
	char **p;
	p = (char**)tabcoloumname[tabno];
	for(i=0;i<tabNO_ColumnTotal[tabno];i++)	  /* 如果字旧段数不够，不够的字段放空 */
	{
		strcat(sqlcmd, *p);
		if(i < (tabNO_ColumnTotal[tabno]-1))
		{
			strcat(sqlcmd, ",");
		}
		else strcat(sqlcmd, ") ");
		p++;
	}
	strcat(sqlcmd, "select * from ");
	strcat(sqlcmd, tmptab_name[tabno]);
	strcat(sqlcmd,";");

	writelog(ZLOG_SQL, "%s", sqlcmd);

	Exec_SqlCmd(sqlcmd);

	return 0;
}


/*****************************************************************************
* rdtxt_instab
* DESCRIPTION : 把下发的数据库表文件，里头的记录插入到各个表里面，
* @txt_name   : 在临时目录下的接受的文本文件
* @tmp_tabname:在数据库里头，插入到各个表的临时文件，插入完成后，把原来旧的表删除
				  现在的表名更改为新的名字
* @tabname	  : 临时文件更改后的表明
* @Return	  : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int rdtxt_instab(int tabno)
{
	if(tabno > TAB_TOTAL)
	{
		writelog(ZLOG_ERR, "[rdtxt_instab] Invalid argument:%d",tabno);
		return -1;		
	}
	FILE *fp;
	char buf[512];
	memset(buf,0,sizeof(buf));
	strcpy(buf,PATH_TMP);
	strcat((char *)buf,m2l_txtname[tabno]);

	if((fp = fopen(buf, "r")) == NULL)
	{
		writelog(ZLOG_ERR,"The %s was not exist,Error:%s,errno:%d",buf,strerror(errno),errno);
		return -1;
	}

	int res=-1;

	/* 创建事务 */
	char *errmsg;
	res= sqlite3_exec (fd , "begin transaction" , 0 , 0 ,&errmsg );
	if(res != 0)
	{
		fclose(fp);
		writelog(ZLOG_ERR,"begin transaction failed exec!, err_code: %d, err_reason: %s", res, errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	res=chkcreate_tab(tmptab_name[tabno], tab_struct_nokey[tabno]);

	if(res != 0)
	{
		fclose(fp);
		writelog(ZLOG_ERR,"failed exec!, err_code: %d, err_reason: %s", res, errmsg);
		sqlite3_free(errmsg);
		return -1;
	}
	while(!feof(fp))
	{
		memset(buf, 0, sizeof(buf));
		fgets((char *)buf, sizeof(buf)+1, fp); //读取了一行(包括行尾的'\r\n',并自动加上字符串结束符'\0')。
		//printf("%s",buf);
		//res=InsLocDB_TmpTab(db,tmp_tabname,buf);
		//sleep(1);
		res=InsLocDB_TmpTab(tabno, buf);
		if(res != 0)
		{
			/* 事务回滚 */
			sqlite3_exec (fd , "rollback transaction" , 0 , 0 , & errmsg );
			fclose(fp);

			writelog(ZLOG_ERR,"rollback transaction failed exec!, err_code: %d, err_reason: %s", res, errmsg);
			sqlite3_free(errmsg);
			return -1;
		}

	}
	/* 提交事务 */
	res = sqlite3_exec (fd ,"commit transaction" , 0 , 0 , & errmsg );
	if(res != 0)
	{
		fclose(fp);

		writelog(ZLOG_ERR,"commit transaction failed exec!, err_code: %d, err_reason: %s", res, errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	fclose(fp);
	sqlite3_free(errmsg);
	updatetab_allrec(tabno);

	/* 删除临时表格 */
	memset(buf, 0, sizeof(buf));
	droptab_cmd(tmptab_name[tabno],buf);
	writelog(ZLOG_SQL, "%s", buf);
	res=Exec_SqlCmd(buf);
	if(res != 0)
	{
		writelog(ZLOG_ERR,"delete %s failed exec!,err_code: %d",tmptab_name[tabno],res);
		return -1;
	}
	return 0;

}

/**************************************************************************
* 函数名称：performance_freespace
* 功能描述：对数据库中的空间进行释放，提升性能
* 输入参数：无
* 输出参数：无
* 返 回 值：int:	-1:操作不成功0:操作成功
* 其它说明：
* 修改时间:	2015-06-23
**************************************************************************/
int performance_freespace(void)
{
	char sqlcmd[256];
	memset(sqlcmd, 0, sizeof(sqlcmd));

	strcat(sqlcmd, "vacuum;");

	writelog(ZLOG_SQL, "%s", sqlcmd);

	char *errmsg = NULL;
	int res = -1;
	res = sqlite3_exec(fd, sqlcmd, NULL, NULL, &errmsg);
	if( res != SQLITE_OK )
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s", res, sqlite3_errmsg(fd));
		sqlite3_free(errmsg);
		return -1;
	}
	sqlite3_free(errmsg);

	return 0;
}

/*****************************************************************************
* Sqlite3_Init
* DESCRIPTION: inition the sqlite3 system,check database and tables,
* @Param  : three
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int database_init(void)
{
	unsigned char i=0;
	int res=-1;
	unsigned char buf[256];
	memset(buf,0,sizeof(buf));
	sqlite3_initialize();
	res=sqlite3_open(PATH_DATABASE,&fd);/* 如果数据库不存在，则自动建立一个 */
	if(res)
	{
		writelog(ZLOG_ERR,"err_code: %d,Can't open database:%s",res,sqlite3_errmsg(fd));
		sqlite3_close(fd);
		return -1;
	}
	res=-1;

	/* 检查表格 */
	for(i=0; i<TAB_TOTAL; i++)
	{
		chkcreate_tab(tab_name[i], tab_struct[i]);
	}

    //对数据库中的空间进行释放，提升性能
    performance_freespace(); 
	return 0;
}

/*****************************************************************************
* get_tabtotal
* DESCRIPTION: get gateway total
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int get_tabtotal(char *sqlcmd, int *total)
{
#if 0
	if((sqlcmd == NULL)||(total == NULL))
	{
		writelog(ZLOG_ERR, "[get_tabtotal] Invalid argument!");
		return -1;		
	}

	char *errmsg;
	int res=-1;
	int ncols=-1;
	sqlite3_stmt *stmt;
	//res = sqlite3_exec(fd, sqlcmd,(int)strlen(sqlcmd), &stmt, &errmsg);
	res= sqlite3_prepare(fd, sqlcmd, (int)strlen(sqlcmd), &stmt, &errmsg);
	if (res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_free(errmsg);
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_free(errmsg);

	ncols = sqlite3_column_count(stmt);
	if(ncols != 1)
	{
		writelog(ZLOG_ERR,"The database has two same name table,please check it");
		sqlite3_finalize(stmt);
		return -1;
	}

	if(res == SQLITE_ROW)//_by_pbh_2014-01-03数据库检查建表函数sqlite3_step返回值判断
	{
		*total=sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);
#endif	
	return 0;

}


/*****************************************************************************
* get_gwID2list
* DESCRIPTION: get gateway ID and insert to the gwlist
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int get_gwID2list(int fileno, gw_list_s *list)
{
	if((fileno >= TAB_TOTAL)||(list == NULL))
	{
		writelog(ZLOG_ERR, "[get_gwID2list] Invalid argument!");
		return -1;		
	}
	
	char sqlcmd[256];
	char **pcoloumname;
	memset(sqlcmd,0,sizeof(sqlcmd));
	pcoloumname = (char**)tabcoloumname[fileno] + 1;
	sprintf(sqlcmd, "select %s from %s group by %s.%s order by %s",
		*pcoloumname, tab_name[fileno], tab_name[fileno], *pcoloumname,*pcoloumname);
	writelog(ZLOG_SQL,"%s",sqlcmd);


	int res=-1;
	sqlite3_stmt *stmt;
	const char *tail;
	int ncols=-1;

	res= sqlite3_prepare(fd, sqlcmd, (int)strlen(sqlcmd), &stmt, &tail);
	if(res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);	
		return -1;
	}

	ncols = sqlite3_column_count(stmt);
	res = sqlite3_step(stmt);
	while(res == SQLITE_ROW)
	{	
		ins_gwlist(list, sqlite3_column_int(stmt, 0));
		writelog(ZLOG_GW, "获取网关ID:%d",list->Tail->gwID);
		res = sqlite3_step(stmt);	
	}
	sqlite3_finalize(stmt);
	return 0;
}

/*****************************************************************************
* full_gwlistheadinfo
* DESCRIPTION: get gateway ID and insert to the gwlist
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int full_gwlistheadinfo(int fileno,gw_list_s *q)
{
	if((q == NULL)||(fileno >= TAB_TOTAL))
	{
		writelog(ZLOG_ERR, "[full_gwlistheadinfo] Invalid argument!");
		return -1;		
	}
	
	char sqlcmd[256];
	char **pparkID;
	char **pgarageID;	
	memset(sqlcmd,0,sizeof(sqlcmd));
	pparkID = (char**)tabcoloumname[fileno] + 0;
	pgarageID = (char**)tabcoloumname[fileno] + 1;

	sprintf(sqlcmd, "select %s,%s from %s limit 1",
		  *pparkID, *pgarageID, tab_name[fileno]);
	writelog(ZLOG_SQL,"%s",sqlcmd);


	int res=-1;
	sqlite3_stmt *stmt;
	const char *tail;
	int ncols=-1;

	res= sqlite3_prepare(fd, sqlcmd, (int)strlen(sqlcmd), &stmt, &tail);
	if(res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);	
		return -1;
	}

	ncols = sqlite3_column_count(stmt);
	res = sqlite3_step(stmt);
	/* 获取网关节点IP信息 */
	if(res != SQLITE_ROW)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);	
		return -1;
	}
	q->ParkId= sqlite3_column_int(stmt, 0);
	q->GarageId= sqlite3_column_int(stmt, 1);
	
	sqlite3_finalize(stmt);
	return 0;
}

/*****************************************************************************
* get_gwID2list
* DESCRIPTION: get gateway ID and insert to the gwlist
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int full_gwIDinfo(gw_node_s *n,int fileno)
{
	if((n == NULL)||(fileno >= TAB_TOTAL))
	{
		writelog(ZLOG_ERR, "[full_gwIDinfo] Invalid argument!");
		return -1;		
	}
	
	char sqlcmd[256];
	char **pgwID;
	char **pSpaceID;	
	memset(sqlcmd,0,sizeof(sqlcmd));
	pgwID = (char**)tabcoloumname[fileno] + 1;
	pSpaceID = (char**)tabcoloumname[fileno] + 5;

	sprintf(sqlcmd, "select * from %s where %s=%d order by %s",
		 tab_name[fileno], *pgwID, n->gwID, *pSpaceID);
	writelog(ZLOG_SQL,"%s",sqlcmd);


	int res=-1;
	sqlite3_stmt *stmt;
	const char *tail;
	int ncols=-1;
	int i=0;
	int j=0;

	res= sqlite3_prepare(fd, sqlcmd, (int)strlen(sqlcmd), &stmt, &tail);
	if(res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);	
		return -1;
	}

	ncols = sqlite3_column_count(stmt);
	res = sqlite3_step(stmt);
	/* 获取网关节点IP信息 */
	if(res == SQLITE_ROW)
	{
		n->gwID = sqlite3_column_int(stmt, 1);
		strcpy(n->gwIP,(char *)sqlite3_column_text(stmt, 2));
		n->gwPort = sqlite3_column_int(stmt, 3);
	}
	/* 获取车位配置具体信息 */
	while(res == SQLITE_ROW)
	{	
		i=sqlite3_column_int(stmt, 5);
		if((i> MAX_GWSPACE_LEN)||(i <= 0))/* 超出范围 */
		{
			res = sqlite3_step(stmt);
			writelog(ZLOG_GW, "[full_gwIDinfo] %d,超出ID值范围",i);
			continue;
		}
		j=0;
		j=i-1;/* 数组从0开始 */
		n->SpaceInfo[j].SpaceID = i;
		strcpy(n->SpaceInfo[j].SpaceNO,(char *)sqlite3_column_text(stmt, 4));
		n->SpaceInfo[j].InCnt = sqlite3_column_int(stmt, 6);
		n->SpaceInfo[j].InUse = sqlite3_column_int(stmt, 7);
		n->SpaceInfo[j].UseLight = sqlite3_column_int(stmt, 8);
		n->SpaceInfo[j].BlueLight = sqlite3_column_int(stmt, 9);
	
		res = sqlite3_step(stmt);		
	}
	sqlite3_finalize(stmt);
	return 0;
}
#if 0
	printf("网关ID=%d,IP=%s,port=%d,  SpaceNO=%s, SpaceID=%d,InCnt=%d, InUse=%d, UseLight=%d,  BlueLight=%d\r\n",

		n->gwID,n->gwIP,n->gwPort,
		n->SpaceInfo[j].SpaceNO,
		n->SpaceInfo[j].SpaceID,
		n->SpaceInfo[j].InCnt,
		n->SpaceInfo[j].InUse,
		n->SpaceInfo[j].UseLight,
		n->SpaceInfo[j].BlueLight
	);
#endif
/*****************************************************************************
* full_gwlistinfo
* DESCRIPTION: get gateway total
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int full_gwlistinfo(gw_list_s *list)
{
	if(list == NULL)
	{
		writelog(ZLOG_ERR, "[full_gwlistinfo] Invalid argument!");
		return -1;		
	}

	gw_node_s *n;
	n = list->Head;
	while(n != NULL)
	{
		if(n->gwID == 0)
		{
			n=n->Next;
			continue;
		}
		writelog(ZLOG_GW, "填充网关ID:%d",n->gwID);
		full_gwIDinfo(n, TABNO_AREAINFO);
		writelog(ZLOG_GW, "填充网关ID:%d 完成",n->gwID);
		n=n->Next;	
	}
	return 0;
}

/*****************************************************************************
* update_spaceinfo2db
* DESCRIPTION: get gateway total
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int update_spaceinfo2db(gw_list_s *list)
{
	if(list == NULL)
	{
		writelog(ZLOG_ERR, "[update_spaceinfo2db] Invalid argument!");
		return -1;		
	}

	gw_node_s *n;
	n = list->Head;
	
	char cds[128];
	char updateinfo[128];
	char **pgwID;
	char **pSpaceNO;
	char **pSpaceID;
	char **pState;
	char **pDevErr;
	int j;
	j=TABNO_AREAINFO;
	pgwID = (char**)tabcoloumname[j] + 1;
	pSpaceNO = (char**)tabcoloumname[j] + 4;
	pSpaceID = (char**)tabcoloumname[j] + 5;
	pState = (char**)tabcoloumname[j] + 10;
	pDevErr = (char**)tabcoloumname[j] + 11;

	while(n != NULL)
	{
		if(n->gwID == 0)
		{
			n=n->Next;
			continue;
		}
		int i;
		for(i=0;i<MAX_GWSPACE_LEN;i++)
		{
			if((n->SpaceInfo[i].SpaceID == 0)||(n->SpaceInfo[i].InUse == 0)
			   ||(n->SpaceInfo[i].NowStat == n->SpaceInfo[i].LastStat))
			{
				continue;
			}

			memset(cds, 0, sizeof(cds));
			memset(updateinfo, 0, sizeof(updateinfo));

			sprintf(updateinfo, "%s=%d,%s=%d", *pState, n->SpaceInfo[i].NowStat,
				*pDevErr,n->SpaceInfo[i].DevErr);
			
			sprintf(cds, "%s='%s' and %s='%d' and %s='%d'",
				*pSpaceNO,n->SpaceInfo[i].SpaceNO,*pgwID,n->gwID,
				*pSpaceID, n->SpaceInfo[i].SpaceID);
			
			update_TabRec(tab_name[j], updateinfo, cds);
			
			n->SpaceInfo[i].LastStat=n->SpaceInfo[i].NowStat;
		}
		n=n->Next;
	}
	return 0;
}

/*****************************************************************************
* get_ledIP
* DESCRIPTION: get led ip
* @Param  : as the list of param
* @Return : if insert OK,return 0,else return -1
* ----
******************************************************************************/
int get_ledIP(int fileno, char *ip)
{
	if((fileno >= TAB_TOTAL)||(ip == NULL))
	{
		writelog(ZLOG_ERR, "[get_ledIP] Invalid argument!");
		return -1;		
	}
	
	char sqlcmd[256];
	char **pcoloumname;
	memset(sqlcmd,0,sizeof(sqlcmd));
	pcoloumname = (char**)tabcoloumname[fileno] + 5;
	sprintf(sqlcmd, "select %s from %s limit 1", *pcoloumname, tab_name[fileno]);
	writelog(ZLOG_SQL,"%s",sqlcmd);

	int res=-1;
	sqlite3_stmt *stmt;
	const char *tail;
	int ncols=-1;

	res= sqlite3_prepare(fd, sqlcmd, (int)strlen(sqlcmd), &stmt, &tail);
	if(res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);	
		return -1;
	}

	ncols = sqlite3_column_count(stmt);
	res = sqlite3_step(stmt);
	if(res == SQLITE_ROW)
	{	
		strcpy(ip,(char *)sqlite3_column_text(stmt, 0));
		writelog(ZLOG_GW, "获取LED的IP是:%s",ip);	
	}
	sqlite3_finalize(stmt);
	return 0;
}



int full_remainspacefromtab(void)
{
	
	char sqlcmd[256];
	char **pgwID;
	char **pSpaceID;	
	memset(sqlcmd,0,sizeof(sqlcmd));
	strcpy(sqlcmd, "select count(*) from areainfo_tab where state=0");
	writelog(ZLOG_SQL,"%s",sqlcmd);


	int res=-1;
	sqlite3_stmt *stmt;
	const char *tail;
	int ncols=-1;
	int i=0;
	int j=0;
        printf("00000000000000000000000000000000\r\n");
	res= sqlite3_prepare(fd, sqlcmd, (int)strlen(sqlcmd), &stmt, &tail);   
	if(res != SQLITE_OK)
	{
		writelog(ZLOG_ERR, "err_code: %d,SQL error: %s",res, sqlite3_errmsg(fd));
		sqlite3_finalize(stmt);	
		return -1;
	}
        printf("11111111111111111111111111111111111111111\r\n");

	ncols = sqlite3_column_count(stmt);
	res = sqlite3_step(stmt);
            printf("222222222222222222222222222222\r\n");

	if(res == SQLITE_ROW)
	{
	            printf("333333333333333333333333\r\n");

		j= sqlite3_column_int(stmt, 0);	//shl_debug_20130813,第二个字段
	}
            printf("44444444444444444\r\n");

	sqlite3_finalize(stmt);
	return j;
}

