#ifndef __LED_H__
#define __LED_H__


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

//uint16 CalcCRC(uint8 *data, uint32 size);

#define Fault 0x00
#define SUCCESS 0
#if 0//�����Ushell_usr.h�г�ͻ�����벻��
#define FAILED 1
#define PARAMETER_ERROR 2
#endif
#define N 256


typedef  struct
{
	int  LedCon_Fd; 		  //LED�����Ӿ��
	int  LEDId; 				 //LED�����
	char IPAddr[20];		  //�鱨��IP��ַ
	int  COMPort;			   //�鱨�崮�ں�
	char LeftAreaId[15];	  //��߷���������
	char FrontAreaId[15];	  //ǰ������������
	char RightAreaId[15];	  //�ұ߷���������
	int  LeftAreaRemains;	  //����ܵ�ʣ�೵λ��
	int  FrontAreaRemains;	  //ǰ���ܵ�ʣ�೵λ��
	int  RightAreaRemains;	  //�ұ��ܵ�ʣ�೵λ��
	char ModifyTime[30];	  //LED����Ϣ�༭ʱ��
}led_stru;


typedef enum cmderror {
	ERR_NO = 0, 						   /* No Error : û�д���*/
	ERR_OUTOFGROUP = 1, 			   /* Command Group Error : ���������Ŵ���*/
	ERR_NOCMD = 2,						  /* Command Not Found : ������û�з���*/
	ERR_BUSY = 3,						 /* The Controller is busy now : �ÿ���������ʹ����*/
	ERR_MEMORYVOLUME = 4,				 /* Out of the Memory Volume : �����ڴ�����*/
	ERR_CHECKSUM = 5,					 /* CRC16 Checksum Error : CRC16 У��ʹ���*/
	ERR_FILENOTEXIST = 6,				 /* File Not Exist : �ļ�������*/
	ERR_FLASH = 7,						  /* Flash Access Error : ������ʴ���*/
	ERR_FILE_DOWNLOAD = 8,			  /* File Download Error : �ļ����ش���*/
	ERR_FILE_NAME = 9,					  /* Filename Error : �ļ�������*/
	ERR_FILE_TYPE = 10, 				   /* File type Error : �ļ����ʹ���*/
	ERR_FILE_CRC16 = 11,				/* File CRC16 Error : �ļ�CRC16 ����*/
	ERR_FONT_NOT_EXIST = 12,			/* Font Library Not Exist : ����ⲻ����*/
	ERR_FIRMWARE_TYPE = 13, 		   /* Firmware Type Error (Check the controller type) : �̼����ʹ���*/
	ERR_DATE_TIME_FORMAT = 14,		  /* Date Time format error : ����ʱ���ʽ����*/
	ERR_FILE_EXIST = 15,				/* File Exist for File overwrite : ���Ǵ����ļ�����*/
	ERR_FILE_BLOCK_NUM = 16,			/* File block number error : �ļ���Ŵ���*/
}cmderror;

typedef enum cmdgroup {
	CMDG_WriteItemFile = 0xA1,							  /* ���ͽ�Ŀ�ļ�����*/
	CMDG_WriteRealitmeDisplayMessage = 0xA3,			/* ����ʵʱ��ʾ��Ϣ����*/
	CMDG_BX5MK = 0xA4,									  /* BX_5MKר������*/
	CMDG_QueryControllerState = 0xA1,					 /* ��ѯ������״̬����*/
	CMDG_Format = 0xA1, 							   /* ��ʽ������*/
	CMDG_DeleteFile = 0xA1, 							   /* ɾ���ļ�����*/
	CMDG_Ping = 0xA2,									 /* ping ����*/
	CMDG_ResetSystem = 0xA2,							/* ϵͳ��λ����*/
	CMDG_UpdateSystemTime = 0xA2,						 /* ϵͳʱ��У������*/
	CMDG_QueryCurrentFirmware = 0xA2,					 /* ��ѯ��ǰ�̼�����*/
	CMDG_ActivateFirmware = 0xA2,						 /* ����̼�����*/
	CMDG_SetScreenID = 0xA2,							/* ������ID ����*/
	CMDG_ReadScreenID = 0xA2,							 /* ��ȡ��ID ����*/
	CMDG_ForceOpenAndClose = 0xA3,						  /* ǿ�ƿ��ػ�����*/
	CMDG_ClockOpenAndClose = 0xA3,						  /* ��ʱ���ػ�����*/
	CMDG_CancelOpenAndClose = 0xA3, 				   /* ȡ����ʱ���ػ�����*/
	CMDG_SetBrightness = 0xA3,							  /* ������������*/
	CMDG_LockORUnlockItme = 0xA3,						 /* ����/������Ŀ����*/
}cmdgroup;

//A1 �������е�������
typedef enum a1cmd {
	CMD_Format = 0x00,									  /* ��ʽ������*/
	CMD_DeleteFile = 0x01,								  /* ɾ���ļ�����*/
	CMD_QueryControllerState = 0x02,						/* ��ѯ������״̬����*/
	CMD_BeginWriteFile = 0x05,							  /* ��ʼд�ļ�����*/
	CMD_WriteOneFile = 0x06,							/* д�ļ�����*/
	CMD_WriteMoreFile = 0x07,							 /* д����ļ�����*/
	CMD_OverWriteFile = 0x08,							 /* ����д���ļ�����*/
}a1cmd;

//A2 �������е�������
typedef enum a2cmd {
	CMD_Ping = 0x00,									/* ping ����*/
	CMD_ResetSystem = 0x01, 						    /* ϵͳ��λ����*/
	CMD_UpdateSystemTime = 0x03,						/* ϵͳʱ��У������*/
	CMD_QueryCurrentFirmware = 0x04,					/* ��ѯ��ǰ�̼�����*/
	CMD_ActivateFirmware = 0x05,						/* ����̼�����*/
	CMD_SetScreenID = 0x06, 							/* ������ID ����*/
	CMD_ReadScreenID = 0x07,							/* ��ȡ��ID ����*/
}a2cmd;

//A3 �������е�������
typedef enum a3cmd {
	CMD_ForceOpenAndClose = 0x00,						 /* ǿ�ƿ��ػ�����*/
	CMD_ClockOpenAndClose = 0x01,						 /* ��ʱ���ػ�����*/
	CMD_SetBrightness = 0x02,							 /* ������������*/
	CMD_LockORUnlockItme = 0x04,						/* ����/������Ŀ����*/
	CMD_WriteRealitmeDisplayMessage = 0x06, 		   /* ����ʵʱ��ʾ��Ϣ����*/
	CMD_CancelOpenAndClose = 0x08,						  /* ȡ����ʱ���ػ�����*/
}a3cmd;

//A4 �������е�������
typedef enum a4cmd {
	CMD_SetSpeAdaDyArea = 0x01, 					   /* ��������Ӧ�ö�̬��*/
	CMD_WritePageData = 0x02,							 /* ���ͷ�ҳ����*/
	CMD_WritePointMess = 0x03,							  /* ���͵�����Ϣ*/
	CMD_DeleteSpeAdaDyArea = 0x04,						  /* ɾ������Ӧ�ö�̬��*/
	CMD_SetIpAddr = 0x05,								 /* ����IP��ַ*/
	CMD_SetMacAddr = 0x06,								  /* ����MAC��ַ*/
	CMD_WebSearch = 0x07,								 /* ��������*/
	CMD_WebHeartbeat = 0x08,							/* ��������*/
	CMD_DeletePageData = 0x09,							  /* ɾ��ҳ����*/
}a4cmd;

//�Ƿ�Ҫ��������ظ�
typedef enum response {
	MustResponse = 0x01,								/* ����������ظ�*/
	UnResponse = 0x02,									  /* ���������ػظ�*/
}response;

//�ļ����Ƿ�ʽ
typedef enum overwrite {
	UnWriteOverFile = 0x00, 	   /* ���ļ�ϵͳ���Ѿ����ڸ��ļ��������·����ļ������� ERR_FILE_EXIST ״̬*/
	WriteOverFile = 0x01,			 /* ���ļ�ϵͳ���Ѿ����ڸ��ļ�����ֱ�Ӹ��Ǹ��ļ�*/
}overwrite;

//�豸����
typedef enum devicetype {
	Wildcard = 0xFE,
	BX_5K1 = 0x51,
	BX_5K2 = 0X58,
	BX_5MK2 = 0x52,
	BX_5MK1 = 0x54,
}devicetype;

//��ͷ���ݸ�ʽ
typedef struct datapackethead {
	uint8 DstAddr[2];						 /* ����ַ0x0001 */
	uint8 SrcAddr[2];						 /* Դ��ַ0x8000 */
	uint8 Reserved[6];					  /* ����0x00 00 00 00 00 00 */
	uint8 DeviceType;					 /* �豸����0xFE�����豸����ͨ���0x51����BX-5K1 0x58����BX-5K2 0x53����BX-5MK2 0x54����BX-5MK1 */
	uint8 ProtocolVersion;				  /* Э��汾��0x02 */
	uint8 DataLen[2];						 /* �����򳤶�0x00 00 */
}datapackethead;

//�������ݸ�ʽ
typedef struct fielddataframe{
	uint8 AreaType; 			   /* ��������*/
	uint8 AreaX[2]; 				   /* ���� X ���꣬���ֽ�(8 �����ص�)Ϊ��λ*/
	uint8 AreaY[2]; 				 /* ���� Y ���꣬�����ص�Ϊ��λ*/
	uint8 AreaWidth[2]; 		   /* �����ȣ����ֽ�(8 �����ص�)Ϊ��λ*/
	uint8 AreaHeight[2];			/* ����߶ȣ������ص�Ϊ��λ*/
	uint8 DynamicAreaLoc;		 /* ��̬������ */
	uint8 Lines_sizes;				 /* �м��*/
	uint8 RunMode;				   /* ��̬������ģʽ
									0�� ��̬������ѭ����ʾ��
									1�� ��̬��������ʾ��ɺ�ֹ��ʾ���һҳ���ݡ�
									2�� ��̬������ѭ����ʾ�������趨ʱ���������δ����ʱɾ����̬����Ϣ��
									3�� ��̬������ѭ����ʾ�������趨ʱ���������δ����ʱ��ʾ Logo ��Ϣ��
								*/
	uint8 Timeout[2];				 /* ��̬�����ݳ�ʱʱ�䣬��λΪ��*/
	uint8 Reserved[3];			  /* ������*/
	uint8 SingleLine;				 /* �Ƿ�����ʾ0x01����������ʾ0x02����������ʾ*/
	uint8 NewLine;				  /* �Ƿ��Զ�����
									0x01�������Զ����У���ʾ�����ڻ���ʱ������뻻�з�
									0x02�����Զ����У���ʾ���ݲ���Ҫ���з�������ֻ��ʹ��ͳһ�����������Ӣ������
								*/
	uint8 DisplayMode;			  /* ��ʾ��ʽ���䶨�����£�
									0x01������ֹ��ʾ
									0x02�������ٴ��
									0x03���������ƶ�
									0x04���������ƶ�
									0x05���������ƶ�
									0x06���������ƶ�
								*/
	uint8 ExitMode; 			   /*  �˳���ʽ*/
	uint8 Speed;					/* ��ʾ�ٶȣ��������£�
									0x00�������
									0x01����
									...
									0x17����
									0x18 ��������
								*/
	uint8 StayTime; 			   /* ��ʾͣ��ʱ�䣬��λΪ 0.5s*/
	uint8 DataLen[4];				 /* ���ݳ��ȣ��������У���ɫ��ת�������*/
	uint8 *Data;					/* ��ʾ���ݲο�ͼ�������ݱ༭*/
}fielddataframe;

typedef struct fielddataframeL2{
	uint8 AreaDataLen[2];				 /* ���������ݳ���*/
	fielddataframe fldtfm;				  /* ��������*/
}fielddataframeL2;

typedef struct fielddataframeL4{
	uint8 AreaDataLen[4];				 /* ���������ݳ���*/
	fielddataframe fldtfm;				  /* ��������*/
}fielddataframeL4;

//��Ŀ�ļ���ʽ
typedef struct itemfilestyle{
	uint8 FileType; 				   /* �ļ�����*/
	uint8 FileName[4];				  /* �ļ���PXXX��XXX Ϊ�ļ���ţ�ASCII ���ʾ
									�ļ���Ϊ�ַ��������Ͱ�˳���ͣ��硰P123�����ȷ��͡�P��������͡�3����
									���� LOGO �ļ���Ϊ��LOGO����
									*/
	uint8 FileLen[4];					 /* �ļ�����*/
	uint8 Reserved; 				   /* ������*/
	uint8 DisplayType[2];				 /* ��Ŀ���ŷ�ʽ
									0����˳�򲥷�
									���������������ŵ�ʱ�䣬��λΪ��
									*/
	uint8 PlayTimes;					/* ��Ŀ�ظ����Ŵ��� */
	uint8 ProgramLife[8];				 /* ��Ŀ�������ڣ�����˳��Ϊ����ʼ��(2)+��ʼ��(1)+��ʼ��(1)+ ������(2)+������(1)+������(1)
									ע��1.	ʱ������� BCD ��ķ�ʽ2.  �귶ΧΪ 0x1900��0x2099��0xffff Ϊ������Ч���ȷ��� LSB������ MSB
									*/
	uint8 ProgramWeek;				  /* ��Ŀ����������
									1.	Bit0 Ϊ 1 ��ʾһ���е�ÿһ�춼���š�
									2.	Bit0 Ϊ 0 ʱ�����ж� bit1-bit7 ��������ÿ�첥�ţ�bit1-bit7 ���α�ʾ��һ�����ա�
									3.	����Ϊ 0 ��ʾ��ֹ���ţ�Ϊ 1 ��ʾ���š�
									*/
	uint8 Reserved2[2]; 			   /* ������*/
	uint8 AreaNum;					  /* �������*/
	#if 0
	uint8 AreaDataLen0[4];			  /* ���� 0 ���ݳ���*/
	uint8 *AreaData0;				 /* ���� 0 ���� ���ο��������ݸ�ʽ*/
	uint8 AreaDataLenN[4];			  /* ���� N ���ݳ���*/
	uint8 *AreaDataN;				 /* ���� N ���ݣ��ο��������ݸ�ʽ*/
	#endif
	fielddataframeL4 *AreaLenAndData[N];/* �������ݳ��Ⱥ���������*/
	uint8 CHK[2];						 /* ������Ŀ�ļ��� CRC16 У��ֵ*/
}itemfilestyle;

//��ʼд�ļ��������ʽ
typedef struct bgwrtfl{
	uint8 CmdGroup; 					   /* ���������0xXX*/
	uint8 Cmd;							  /* ������0xXX*/
	uint8 Response; 					   /* �Ƿ�Ҫ��������ظ���0x01��������������ظ�0x02�������������ػظ�*/
	uint8 Reserved[2];					  /* ����0x00 00*/
	uint8 OverWrite;						/* �ļ����Ƿ�ʽ*/
	uint8 FileName[4];					  /* �ļ���*/
	uint8 FileLength[4];					/* �ļ�����*/
}bgwrtfl;

//д�ļ��������ʽ
typedef struct wrtonefl{
	uint8 CmdGroup; 					   /* ���������0xXX*/
	uint8 Cmd;							  /* ������0xXX*/
	uint8 Response; 					   /* �Ƿ�Ҫ��������ظ���0x01��������������ظ�0x02�������������ػظ�*/
	uint8 Reserved[2];					  /* ����0x00 00*/
	uint8 FileName[4];					  /* �ļ���*/
	uint8 LastBlockFlag;					/* ��־�Ƿ����һ����0x00-- �������һ����0x01--���һ��*/
	uint8 BlockNum[2];					  /* ���ţ�����ǵ������ͣ���Ĭ��Ϊ 0x00*/
	uint8 BlockLen[2];					  /* ���������ǵ������ͣ��˴�Ϊ�ļ�����*/
	uint8 BlockAddr[4]; 				   /* �����������ļ��е���ʼλ�ã�����ǵ������ͣ��˴�Ĭ��Ϊ 0*/
	itemfilestyle *flData;				  /* �ļ������ݣ��ο���Ŀ�ļ���ʽ*/
}wrtonefl;

//����ʵʱ��ʾ��Ϣ�������ʽ
typedef struct wrtrldispmsg{
	uint8 CmdGroup; 					   /* ���������0xXX*/
	uint8 Cmd;							  /* ������0xXX*/
	uint8 Response; 					   /* �Ƿ�Ҫ��������ظ���0x01��������������ظ�0x02�������������ػظ�*/
	uint8 Reserved[2];					  /* ����0x00 00*/
	uint8 DeleteAreaNum;					/* Ҫɾ�������������
										ע�⣺�����ֵΪ 0xFF����ɾ�����ж�̬�����ݡ�
										�����ֵΪ 0x00����ɾ������
										*/
	//uint8 *DeleteAreaId;					  /* ��Ҫɾ�������� ID �����Ҫɾ�����������Ϊ 0����������*/
	uint8 AreaNum;						  /* ������������θ��µ����������*/
	fielddataframeL2 *AreaLenAndData[N];/* �������ݳ��Ⱥ���������*/
}wrtrldispmsg;

#if 0
//�������ʽ
typedef struct datafield {
	uint8 CmdGroup[1];					  /* ���������0xXX*/
	uint8 Cmd;							  /* ������0xXX*/
	union RequAndResp {
		uint8 Response; 				   /* �Ƿ�Ҫ��������ظ���0x01��������������ظ�0x02�������������ػظ�*/
		uint8 CmdError; 				   /* �����״̬0xXX*/
		}RequAndResp;
	uint8 Reserved[2];					  /* ����0x00 00*/
	/* ���͵�����N��0xXX*/
	#if 0
	union fData {
		bgwrtfl bwf;
		}Data;
	#endif
	uint8 *fData;
}datafield;
#endif

// ��׼ͨѶ��ʽ
typedef struct protocolform {
	uint8 framehead[8]; 				   /* ֡ͷ*/
	datapackethead dtpckh;				  /* ��ͷ����*/
	union dtfld{						/* ������*/
		bgwrtfl *bwf;					 /* ��ʼд�ļ�������*/
		wrtrldispmsg *wtdm; 			   /* ����ʵʱ��ʾ��Ϣ������*/
		wrtonefl *wrf;					/*д�ļ�������*/
	}dtfld;
	uint8 crc16checksum[2]; 			   /* ��У��*/
	uint8 frametail;					/* ֡β*/
}protocolform;

/*----------------------------��׼ͨѶ��ʽ( = 25+N �ֽ�)-------------------------------------------------------

|--------------------------------------------------------------------------------------------------- |
|	 ֡�ṹ    |				|					 |					   |				|			  |
|				 |	  ֡ͷ		  |    ��ͷ����    |		������	   |	��У��	  | 	֡β	 |
|				 |<-- 8 �ֽ�-->|<----14�ֽ�---->|<----	N �ֽ�---->|<-- 2�ֽ�-->|<-1�ֽ�->|
|				 |				  | 				   |					 |				  | 			|
|--------------------------------------------------------------------------------------------------- |
|	 ����		 | 0xA5 0xA5 0xA5 | 0xXX 0xXX 0xXX		  | 0xXX 0xXX 0xXX ...	   |				|			  |
|				 | 0xA5 0xA5 0xA5 | 0xXX 0xXX 0xXX		  | 0xXX 0xXX 0xXX ...		| 0xXX		0xXX	|	 0x5A	  |
|				 | 0xA5 0xA5		 | 0xXX 0xXX 0xXX ....	  | 0xXX 0xXX 0xXX ...	   |				|			  |
|--------------------------------------------------------------------------------------------------- |

//��ʼд�ļ���A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 0E 00 \\ A1 05 01 00 00 01 50 30 30 30 49 00 00 00 \\ E2 16  \\ 5A
//������Ŀ��A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 0B 00 \\ A3 04 01 06 00 00 01 50 30 30 30 \\ 42 6A \\ 5A
//������Ŀ��A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 0B 00 \\ A3 04 01 06 00 00 00 50 30 30 30 \\ 7F AA \\ 5A
//PING	  ���A5 A5 A5 A5 A5 A5 A5 A5 \\ 01 00 00 80 00 00 00 00 00 00 FE 02 05 00 \\ A2 00 01 00 00 \\ 68 F8 \\ 5A
-----------------------------------------------------------------------------------------------------------------------*/

typedef struct ackornack {
	uint8 CmdGroup; 		   /* ���������0xA0 */
	uint8 Cmd;				  /* ������ack:0x00 nack:0x01*/
	uint8 CmdError; 		  /* �����״̬*/
	uint8 *Reserved;			/* ����0x0000*/
}ackornack;

extern uint8 *wrtReltDspMsg(uint8 *Dispcten, int Len);		 /* ����ʵʱ��ʾ��Ϣ*/
extern unsigned int worldWrite(uint8 *p,uint32 value);		 /*��λת��λ*/
extern int wrt2LEDMsgCar(uint8 *Dispcten,void *LedPara);
extern unsigned int tcpip2Disp_Write(uint8 *Writebuf, int *Sock);				 /* �����������������*/
extern int check_LedConnect(led_stru *SingLedInfo);
extern int setLED_num(uint8 *ledip,uint32 ledport,uint8 * dispcten);


#endif
//����ʵʱ��ʾ��Ϣ
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

//д�ļ�
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
//00 ------�ļ�����
//50 30 30 30 -----�ļ���
//49 00 00 00 ------�ļ�����
//FF -----������
//00 00 -----��Ŀ���ŷ�ʽ
//01 -----��Ŀ�ظ����Ŵ���
//FF FF 01 24 14 20 01 24 ------��Ŀ����������
//FF ----��Ŀ����������
//00 00 -----������
//01 -----�������
//2A 00 00 00 ------���ݳ���

//filed
//00 ---��������
//18 00 ---����X����
//00 00 ---����Y����
//08 00 ---������
//40 00 ---����߶�
//FF ---��̬������
//00 ---�м��
//00 ---��̬������ģʽ
//00 00 ---��̬�����ݳ�ʱʱ��
//00 00 00 ---������
//02 ---������ʾ
//02 ---�Զ�����
//01 ---��ֹ��ʾ
//00 ---�˳���ʽ
//00 ---�����ʾ�ٶ�
//0A ---��ʾͣ��ʱ�䣬��λΪ0.5s
//0F 00 00 00 ---���ݳ���

//content
//77 77 77 2E 6F 6E 62 6F 6E 62 78 2E 63 6F 6D ----www.onbonbx.com
//52 4D -----������Ŀ�ļ���CRC16У��ֵ

//07 11
//5A

