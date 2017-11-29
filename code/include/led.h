#ifndef __LED_H__
#define __LED_H__


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

//uint16 CalcCRC(uint8 *data, uint32 size);

#define Fault 0x00
#define SUCCESS 0
#if 0//定义跟Ushell_usr.h中冲突，编译不过
#define FAILED 1
#define PARAMETER_ERROR 2
#endif
#define N 256


typedef  struct
{
	int  LedCon_Fd; 		  //LED屏连接句柄
	int  LEDId; 				 //LED屏编号
	char IPAddr[20];		  //情报板IP地址
	int  COMPort;			   //情报板串口号
	char LeftAreaId[15];	  //左边发布区域编号
	char FrontAreaId[15];	  //前方发布区域编号
	char RightAreaId[15];	  //右边发布区域编号
	int  LeftAreaRemains;	  //左边总的剩余车位数
	int  FrontAreaRemains;	  //前方总的剩余车位数
	int  RightAreaRemains;	  //右边总的剩余车位数
	char ModifyTime[30];	  //LED屏信息编辑时间
}led_stru;


typedef enum cmderror {
	ERR_NO = 0, 						   /* No Error : 没有错误*/
	ERR_OUTOFGROUP = 1, 			   /* Command Group Error : 命令分组组号错误*/
	ERR_NOCMD = 2,						  /* Command Not Found : 命令编号没有发现*/
	ERR_BUSY = 3,						 /* The Controller is busy now : 该控制器正在使用中*/
	ERR_MEMORYVOLUME = 4,				 /* Out of the Memory Volume : 超出内存容量*/
	ERR_CHECKSUM = 5,					 /* CRC16 Checksum Error : CRC16 校验和错误*/
	ERR_FILENOTEXIST = 6,				 /* File Not Exist : 文件不存在*/
	ERR_FLASH = 7,						  /* Flash Access Error : 闪存访问错误*/
	ERR_FILE_DOWNLOAD = 8,			  /* File Download Error : 文件下载错误*/
	ERR_FILE_NAME = 9,					  /* Filename Error : 文件名错误*/
	ERR_FILE_TYPE = 10, 				   /* File type Error : 文件类型错误*/
	ERR_FILE_CRC16 = 11,				/* File CRC16 Error : 文件CRC16 错误*/
	ERR_FONT_NOT_EXIST = 12,			/* Font Library Not Exist : 字体库不存在*/
	ERR_FIRMWARE_TYPE = 13, 		   /* Firmware Type Error (Check the controller type) : 固件类型错误*/
	ERR_DATE_TIME_FORMAT = 14,		  /* Date Time format error : 日期时间格式错误*/
	ERR_FILE_EXIST = 15,				/* File Exist for File overwrite : 覆盖存在文件错误*/
	ERR_FILE_BLOCK_NUM = 16,			/* File block number error : 文件块号错误*/
}cmderror;

typedef enum cmdgroup {
	CMDG_WriteItemFile = 0xA1,							  /* 发送节目文件命令*/
	CMDG_WriteRealitmeDisplayMessage = 0xA3,			/* 发送实时显示信息命令*/
	CMDG_BX5MK = 0xA4,									  /* BX_5MK专属命令*/
	CMDG_QueryControllerState = 0xA1,					 /* 查询控制器状态命令*/
	CMDG_Format = 0xA1, 							   /* 格式化命令*/
	CMDG_DeleteFile = 0xA1, 							   /* 删除文件命令*/
	CMDG_Ping = 0xA2,									 /* ping 命令*/
	CMDG_ResetSystem = 0xA2,							/* 系统复位命令*/
	CMDG_UpdateSystemTime = 0xA2,						 /* 系统时间校正命令*/
	CMDG_QueryCurrentFirmware = 0xA2,					 /* 查询当前固件命令*/
	CMDG_ActivateFirmware = 0xA2,						 /* 激活固件命令*/
	CMDG_SetScreenID = 0xA2,							/* 设置屏ID 命令*/
	CMDG_ReadScreenID = 0xA2,							 /* 读取屏ID 命令*/
	CMDG_ForceOpenAndClose = 0xA3,						  /* 强制开关机命令*/
	CMDG_ClockOpenAndClose = 0xA3,						  /* 定时开关机命令*/
	CMDG_CancelOpenAndClose = 0xA3, 				   /* 取消定时开关机命令*/
	CMDG_SetBrightness = 0xA3,							  /* 设置亮度命令*/
	CMDG_LockORUnlockItme = 0xA3,						 /* 锁定/解锁节目命令*/
}cmdgroup;

//A1 命令组中的命令编号
typedef enum a1cmd {
	CMD_Format = 0x00,									  /* 格式化命令*/
	CMD_DeleteFile = 0x01,								  /* 删除文件命令*/
	CMD_QueryControllerState = 0x02,						/* 查询控制器状态命令*/
	CMD_BeginWriteFile = 0x05,							  /* 开始写文件命令*/
	CMD_WriteOneFile = 0x06,							/* 写文件命令*/
	CMD_WriteMoreFile = 0x07,							 /* 写多个文件命令*/
	CMD_OverWriteFile = 0x08,							 /* 结束写多文件命令*/
}a1cmd;

//A2 命令组中的命令编号
typedef enum a2cmd {
	CMD_Ping = 0x00,									/* ping 命令*/
	CMD_ResetSystem = 0x01, 						    /* 系统复位命令*/
	CMD_UpdateSystemTime = 0x03,						/* 系统时间校正命令*/
	CMD_QueryCurrentFirmware = 0x04,					/* 查询当前固件命令*/
	CMD_ActivateFirmware = 0x05,						/* 激活固件命令*/
	CMD_SetScreenID = 0x06, 							/* 设置屏ID 命令*/
	CMD_ReadScreenID = 0x07,							/* 读取屏ID 命令*/
}a2cmd;

//A3 命令组中的命令编号
typedef enum a3cmd {
	CMD_ForceOpenAndClose = 0x00,						 /* 强制开关机命令*/
	CMD_ClockOpenAndClose = 0x01,						 /* 定时开关机命令*/
	CMD_SetBrightness = 0x02,							 /* 设置亮度命令*/
	CMD_LockORUnlockItme = 0x04,						/* 锁定/解锁节目命令*/
	CMD_WriteRealitmeDisplayMessage = 0x06, 		   /* 发送实时显示信息命令*/
	CMD_CancelOpenAndClose = 0x08,						  /* 取消定时开关机命令*/
}a3cmd;

//A4 命令组中的命令编号
typedef enum a4cmd {
	CMD_SetSpeAdaDyArea = 0x01, 					   /* 设置特殊应用动态区*/
	CMD_WritePageData = 0x02,							 /* 发送分页数据*/
	CMD_WritePointMess = 0x03,							  /* 发送点阵信息*/
	CMD_DeleteSpeAdaDyArea = 0x04,						  /* 删除特殊应用动态区*/
	CMD_SetIpAddr = 0x05,								 /* 设置IP地址*/
	CMD_SetMacAddr = 0x06,								  /* 设置MAC地址*/
	CMD_WebSearch = 0x07,								 /* 网络搜索*/
	CMD_WebHeartbeat = 0x08,							/* 网络心跳*/
	CMD_DeletePageData = 0x09,							  /* 删除页数据*/
}a4cmd;

//是否要求控制器回复
typedef enum response {
	MustResponse = 0x01,								/* 控制器必须回复*/
	UnResponse = 0x02,									  /* 控制器不必回复*/
}response;

//文件覆盖方式
typedef enum overwrite {
	UnWriteOverFile = 0x00, 	   /* 若文件系统中已经存在该文件，则不再下发该文件，返回 ERR_FILE_EXIST 状态*/
	WriteOverFile = 0x01,			 /* 若文件系统中已经存在该文件，则直接覆盖该文件*/
}overwrite;

//设备类型
typedef enum devicetype {
	Wildcard = 0xFE,
	BX_5K1 = 0x51,
	BX_5K2 = 0X58,
	BX_5MK2 = 0x52,
	BX_5MK1 = 0x54,
}devicetype;

//包头数据格式
typedef struct datapackethead {
	uint8 DstAddr[2];						 /* 屏地址0x0001 */
	uint8 SrcAddr[2];						 /* 源地址0x8000 */
	uint8 Reserved[6];					  /* 保留0x00 00 00 00 00 00 */
	uint8 DeviceType;					 /* 设备类型0xFE——设备类型通配符0x51——BX-5K1 0x58——BX-5K2 0x53——BX-5MK2 0x54——BX-5MK1 */
	uint8 ProtocolVersion;				  /* 协议版本号0x02 */
	uint8 DataLen[2];						 /* 数据域长度0x00 00 */
}datapackethead;

//区域数据格式
typedef struct fielddataframe{
	uint8 AreaType; 			   /* 区域类型*/
	uint8 AreaX[2]; 				   /* 区域 X 坐标，以字节(8 个像素点)为单位*/
	uint8 AreaY[2]; 				 /* 区域 Y 坐标，以像素点为单位*/
	uint8 AreaWidth[2]; 		   /* 区域宽度，以字节(8 个像素点)为单位*/
	uint8 AreaHeight[2];			/* 区域高度，以像素点为单位*/
	uint8 DynamicAreaLoc;		 /* 动态区域编号 */
	uint8 Lines_sizes;				 /* 行间距*/
	uint8 RunMode;				   /* 动态区运行模式
									0— 动态区数据循环显示。
									1— 动态区数据显示完成后静止显示最后一页数据。
									2— 动态区数据循环显示，超过设定时间后数据仍未更新时删除动态区信息。
									3— 动态区数据循环显示，超过设定时间后数据仍未更新时显示 Logo 信息。
								*/
	uint8 Timeout[2];				 /* 动态区数据超时时间，单位为秒*/
	uint8 Reserved[3];			  /* 保留字*/
	uint8 SingleLine;				 /* 是否单行显示0x01——单行显示0x02——多行显示*/
	uint8 NewLine;				  /* 是否自动换行
									0x01——不自动换行，显示数据在换行时必须插入换行符
									0x02——自动换行，显示内容不需要换行符，但是只能使用统一的中文字体和英文字体
								*/
	uint8 DisplayMode;			  /* 显示方式，其定义如下：
									0x01——静止显示
									0x02——快速打出
									0x03——向左移动
									0x04——向右移动
									0x05——向上移动
									0x06——向下移动
								*/
	uint8 ExitMode; 			   /*  退出方式*/
	uint8 Speed;					/* 显示速度，定义如下：
									0x00——最快
									0x01——
									...
									0x17——
									0x18 ——最慢
								*/
	uint8 StayTime; 			   /* 显示停留时间，单位为 0.5s*/
	uint8 DataLen[4];				 /* 数据长度（包括换行，颜色等转义参数）*/
	uint8 *Data;					/* 显示数据参考图文区内容编辑*/
}fielddataframe;

typedef struct fielddataframeL2{
	uint8 AreaDataLen[2];				 /* 该区域数据长度*/
	fielddataframe fldtfm;				  /* 区域数据*/
}fielddataframeL2;

typedef struct fielddataframeL4{
	uint8 AreaDataLen[4];				 /* 该区域数据长度*/
	fielddataframe fldtfm;				  /* 区域数据*/
}fielddataframeL4;

//节目文件格式
typedef struct itemfilestyle{
	uint8 FileType; 				   /* 文件类型*/
	uint8 FileName[4];				  /* 文件名PXXX，XXX 为文件编号，ASCII 码表示
									文件名为字符串，发送按顺序发送，如“P123”则先发送‘P’，最后发送‘3’。
									开机 LOGO 文件名为“LOGO”。
									*/
	uint8 FileLen[4];					 /* 文件长度*/
	uint8 Reserved; 				   /* 保留字*/
	uint8 DisplayType[2];				 /* 节目播放方式
									0——顺序播放
									其他——定长播放的时间，单位为秒
									*/
	uint8 PlayTimes;					/* 节目重复播放次数 */
	uint8 ProgramLife[8];				 /* 节目生命周期，发送顺序为：起始年(2)+起始月(1)+起始日(1)+ 结束年(2)+结束月(1)+结束日(1)
									注：1.	时间均采用 BCD 码的方式2.  年范围为 0x1900—0x2099，0xffff 为永久有效，先发送 LSB，后发送 MSB
									*/
	uint8 ProgramWeek;				  /* 节目的星期属性
									1.	Bit0 为 1 表示一周中的每一天都播放。
									2.	Bit0 为 0 时，需判断 bit1-bit7 的来决定每天播放，bit1-bit7 依次表示周一到周日。
									3.	比特为 0 表示禁止播放，为 1 表示播放。
									*/
	uint8 Reserved2[2]; 			   /* 保留字*/
	uint8 AreaNum;					  /* 区域个数*/
	#if 0
	uint8 AreaDataLen0[4];			  /* 区域 0 数据长度*/
	uint8 *AreaData0;				 /* 区域 0 数据 ，参考区域数据格式*/
	uint8 AreaDataLenN[4];			  /* 区域 N 数据长度*/
	uint8 *AreaDataN;				 /* 区域 N 数据，参考区域数据格式*/
	#endif
	fielddataframeL4 *AreaLenAndData[N];/* 区域数据长度和区域数据*/
	uint8 CHK[2];						 /* 整个节目文件的 CRC16 校验值*/
}itemfilestyle;

//开始写文件数据域格式
typedef struct bgwrtfl{
	uint8 CmdGroup; 					   /* 命令分组编号0xXX*/
	uint8 Cmd;							  /* 命令编号0xXX*/
	uint8 Response; 					   /* 是否要求控制器回复。0x01——控制器必须回复0x02——控制器不必回复*/
	uint8 Reserved[2];					  /* 保留0x00 00*/
	uint8 OverWrite;						/* 文件覆盖方式*/
	uint8 FileName[4];					  /* 文件名*/
	uint8 FileLength[4];					/* 文件长度*/
}bgwrtfl;

//写文件数据域格式
typedef struct wrtonefl{
	uint8 CmdGroup; 					   /* 命令分组编号0xXX*/
	uint8 Cmd;							  /* 命令编号0xXX*/
	uint8 Response; 					   /* 是否要求控制器回复。0x01——控制器必须回复0x02——控制器不必回复*/
	uint8 Reserved[2];					  /* 保留0x00 00*/
	uint8 FileName[4];					  /* 文件名*/
	uint8 LastBlockFlag;					/* 标志是否最后一包，0x00-- 不是最后一包，0x01--最后一包*/
	uint8 BlockNum[2];					  /* 包号，如果是单包发送，则默认为 0x00*/
	uint8 BlockLen[2];					  /* 包长，若是单包发送，此处为文件长度*/
	uint8 BlockAddr[4]; 				   /* 本包数据在文件中的起始位置，如果是单包发送，此处默认为 0*/
	itemfilestyle *flData;				  /* 文件包数据，参考节目文件格式*/
}wrtonefl;

//发送实时显示信息数据域格式
typedef struct wrtrldispmsg{
	uint8 CmdGroup; 					   /* 命令分组编号0xXX*/
	uint8 Cmd;							  /* 命令编号0xXX*/
	uint8 Response; 					   /* 是否要求控制器回复。0x01——控制器必须回复0x02——控制器不必回复*/
	uint8 Reserved[2];					  /* 保留0x00 00*/
	uint8 DeleteAreaNum;					/* 要删除的区域个数。
										注意：如果该值为 0xFF，则删除所有动态区数据。
										如果该值为 0x00，则不删除区域。
										*/
	//uint8 *DeleteAreaId;					  /* 需要删除的区域 ID 号如果要删除的区域个数为 0，则该项不发送*/
	uint8 AreaNum;						  /* 区域个数，本次更新的区域个数。*/
	fielddataframeL2 *AreaLenAndData[N];/* 区域数据长度和区域数据*/
}wrtrldispmsg;

#if 0
//数据域格式
typedef struct datafield {
	uint8 CmdGroup[1];					  /* 命令分组编号0xXX*/
	uint8 Cmd;							  /* 命令编号0xXX*/
	union RequAndResp {
		uint8 Response; 				   /* 是否要求控制器回复。0x01——控制器必须回复0x02——控制器不必回复*/
		uint8 CmdError; 				   /* 命令处理状态0xXX*/
		}RequAndResp;
	uint8 Reserved[2];					  /* 保留0x00 00*/
	/* 发送的数据N个0xXX*/
	#if 0
	union fData {
		bgwrtfl bwf;
		}Data;
	#endif
	uint8 *fData;
}datafield;
#endif

// 标准通讯格式
typedef struct protocolform {
	uint8 framehead[8]; 				   /* 帧头*/
	datapackethead dtpckh;				  /* 包头数据*/
	union dtfld{						/* 数据域*/
		bgwrtfl *bwf;					 /* 开始写文件数据域*/
		wrtrldispmsg *wtdm; 			   /* 发送实时显示信息数据域*/
		wrtonefl *wrf;					/*写文件数据域*/
	}dtfld;
	uint8 crc16checksum[2]; 			   /* 包校验*/
	uint8 frametail;					/* 帧尾*/
}protocolform;

/*----------------------------标准通讯格式( = 25+N 字节)-------------------------------------------------------

|--------------------------------------------------------------------------------------------------- |
|	 帧结构    |				|					 |					   |				|			  |
|				 |	  帧头		  |    包头数据    |		数据域	   |	包校验	  | 	帧尾	 |
|				 |<-- 8 字节-->|<----14字节---->|<----	N 字节---->|<-- 2字节-->|<-1字节->|
|				 |				  | 				   |					 |				  | 			|
|--------------------------------------------------------------------------------------------------- |
|	 数据		 | 0xA5 0xA5 0xA5 | 0xXX 0xXX 0xXX		  | 0xXX 0xXX 0xXX ...	   |				|			  |
|				 | 0xA5 0xA5 0xA5 | 0xXX 0xXX 0xXX		  | 0xXX 0xXX 0xXX ...		| 0xXX		0xXX	|	 0x5A	  |
|				 | 0xA5 0xA5		 | 0xXX 0xXX 0xXX ....	  | 0xXX 0xXX 0xXX ...	   |				|			  |
|--------------------------------------------------------------------------------------------------- |

//开始写文件：A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 0E 00 \\ A1 05 01 00 00 01 50 30 30 30 49 00 00 00 \\ E2 16  \\ 5A
//锁定节目：A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 0B 00 \\ A3 04 01 06 00 00 01 50 30 30 30 \\ 42 6A \\ 5A
//解锁节目：A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 0B 00 \\ A3 04 01 06 00 00 00 50 30 30 30 \\ 7F AA \\ 5A
//PING	  命令：A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 05 00 \\ A2 00 01 00 00 \\ 68 F8 \\ 5A
-----------------------------------------------------------------------------------------------------------------------*/

typedef struct ackornack {
	uint8 CmdGroup; 		   /* 命令分组编号0xA0 */
	uint8 Cmd;				  /* 命令编号ack:0x00 nack:0x01*/
	uint8 CmdError; 		  /* 命令处理状态*/
	uint8 *Reserved;			/* 保留0x0000*/
}ackornack;

extern uint8 *wrtReltDspMsg(uint8 *Dispcten, int Len);		 /* 发送实时显示信息*/
extern unsigned int worldWrite(uint8 *p,uint32 value);		 /*高位转低位*/
extern int wrt2LEDMsgCar(uint8 *Dispcten,void *LedPara);
extern unsigned int tcpip2Disp_Write(uint8 *Writebuf, int *Sock);				 /* 发送命令给服务器端*/
extern int check_LedConnect(led_stru *SingLedInfo);
extern int setLED_num(uint8 *ledip,uint32 ledport,uint8 * dispcten);


#endif
//发送实时显示信息
//A5 A5 A5 A5 A5 A5 A5 A5
//01 00 00 80 00 00 00 00 00 00 FE 02 32 00 (50 byte datafiled)
//datafiled
//A3 06 01 2D 00 00 01 29 00 (41 byte fileddata)
//fileddata
//00
//00 00
//00 00
//18 00
//20 00
//00
//00
//00
//02 00
//00 00 00
//02
//02
//03
//00
//04
//05
//0E 00 00 00
//30 31 32 33 34 35 51 52 53 54 54 56 27 42
//A8 7D
//5A

//写文件
//A5 A5 A5 A5 A5 A5 A5 A5
//01 00 00 80 00 00 00 00 00 00 FE 02 5B 01

//00
//A1
//06
//01
//00 00
//50 30 30 30
//01
//00 00
//49 00
//00 00 00 00

//data
//00 ------文件类型
//50 30 30 30 -----文件名
//49 00 00 00 ------文件长度
//FF -----保留字
//00 00 -----节目播放方式
//01 -----节目重复播放次数
//FF FF 01 24 14 20 01 24 ------节目的生命周期
//FF ----节目的星期属性
//00 00 -----保留字
//01 -----区域个数
//2A 00 00 00 ------数据长度

//filed
//00 ---区域类型
//18 00 ---区域X坐标
//00 00 ---区域Y坐标
//08 00 ---区域宽度
//40 00 ---区域高度
//FF ---动态区域编号
//00 ---行间距
//00 ---动态区运行模式
//00 00 ---动态区数据超时时间
//00 00 00 ---保留字
//02 ---多行显示
//02 ---自动换行
//01 ---静止显示
//00 ---退出方式
//00 ---最快显示速度
//0A ---显示停留时间，单位为0.5s
//0F 00 00 00 ---数据长度

//content
//77 77 77 2E 6F 6E 62 6F 6E 62 78 2E 63 6F 6D ----www.onbonbx.com
//52 4D -----整个节目文件的CRC16校验值

//07 11
//5A

