#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 	   // for socket
#include <sys/types.h>
#include <netinet/in.h>
#include <malloc.h>
#include "include.h"

#define CRC(crc, byte) (((crc) >> 8 ) ^ tabel[((crc) ^ (unsigned int) (byte)) & 0XFF])

static const uint16 tabel[256] = {
0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};


uint16 calcCRC(uint8 *data, uint32 size)
{
	uint32 i;
	uint16 crc = 0;

	for( i = 0; i < size; i++ )
	{
		crc = CRC(crc, data[i]);
	}

	return crc;
}

/* 文件内部全局变量*/
protocolform ProtclForm;
int client_sk = -1;
int Ldtfld = -1;
int Ldtpckh = -1;
int Lchecksum = -1;
int Lfrahd = -1;
int Lfratl = -1;
int Lswtnum = -1;	//转义字符后的包头+数据域+校验的字节数

   uint8 DspX;			 //区域显示X坐标
   uint8 DspY;			   //区域显示Y坐标
   uint8 DspWdth;		  //区域显示宽度
   uint8 DspHght;		  //区域显示高度
   uint8 DspAreaNum;	 //区域号
   uint8 DspFirm;		 //动态显示方式


/*******************************************************************************
* 函数名  : switchChar
* 功能	  : 转义字符处理函数:A5->A6 02;A6->A6 01;5A->5B 02;5B->5B 01
* 输入	  : int CharNum,待转换数据的字节数;uint8 *CharBuffer,待转换数据数组首地址;
			uint8 *Recvbuffer,转换完成后存储的数组首地址

* 输出	  :
* 返回	  :
*******************************************************************************/
void switchChar(int CharNum, uint8 *CharBuffer, uint8 *Recvbuffer)
{
	int i;
	Lswtnum = 0;
	for(i = 0; i < CharNum; i++)
	{
		if(*CharBuffer == 0xA5)
		{
			Recvbuffer[Lswtnum] = 0xA6;
			Recvbuffer[Lswtnum + 1] = 0x02;
			CharBuffer++;
			Lswtnum = Lswtnum + 2;
		}
		else if(*CharBuffer == 0xA6)
		{
			Recvbuffer[Lswtnum] = 0xA6;
			Recvbuffer[Lswtnum + 1] = 0x01;
			CharBuffer++;
			Lswtnum = Lswtnum + 2;
		}
		else if(*CharBuffer == 0x5A)
		{
			Recvbuffer[Lswtnum] = 0x5B;
			Recvbuffer[Lswtnum + 1] = 0x02;
			CharBuffer++;
			Lswtnum = Lswtnum + 2;
		}
		else if(*CharBuffer == 0x5B)
		{
			Recvbuffer[Lswtnum] = 0x5B;
			Recvbuffer[Lswtnum + 1] = 0x01;
			CharBuffer++;
			Lswtnum = Lswtnum + 2;
		}
		else
		{
			Recvbuffer[Lswtnum] = *CharBuffer;
			CharBuffer++;
			Lswtnum++;
		}
	}
}
/*******************************************************************************
* 函数名  : worldWrite
* 功能	  : (长)整型数转换到数组里面，低字节在前，高字节在后
* 输入	  : uint8 *p,数组指针, uint32 value 待转换的数据

* 输出	  :
* 返回	  : 0
*******************************************************************************/
unsigned int worldWrite(uint8 *p,uint32 value)
{
	*p = value & 0x000000FF;
	p++;
	*p = (value & 0x0000FF00) >> 8;
	p++;
	*p = (value & 0x00FF0000) >>16;
	p++;
	*p = (value & 0xFF000000) >> 24;
	return 0;
}

/*******************************************************************************
* 函数名  : tcpip2Disp_Write
* 功能	  : 将处理好的标准格式数据通过TCP/IP传给LED屏
* 输入	  : void *Writebuf,显示内容起始指针 int *Sock 网络句柄

* 输出	  :
* 返回	  :不成功 -1
*******************************************************************************/

unsigned int tcpip2Disp_Write(uint8 *Writebuf, int *Sock)
{
	if(send(*Sock, Writebuf, Lswtnum + Lfratl + Lfrahd, 0) < 0)
	{
#ifdef DEBUG
		printf("发送到服务器超时\r\n");
#endif
	}
	return -1;
}

/*******************************************************************************
* 函数名  : wrtReltDspMsg
* 功能	  : 向LED屏写实时显示区域
* 输入	  : void *Dispcten,显示内容起始指针 int Len 显示内容长度

* 输出	  :
* 返回	  : 经过处理的数据首地址
*******************************************************************************/

uint8 *wrtReltDspMsg(uint8 *Dispcten, int Len)
{
	//printf("Write display message on real time.\n");

	/*发送实时显示信息数据域格式，7个固定字节*/
	int Ldtfld_wtdm;
	Ldtfld_wtdm = sizeof(wrtrldispmsg);
	ProtclForm.dtfld.wtdm = (wrtrldispmsg *)malloc(Ldtfld_wtdm);
	if( ProtclForm.dtfld.wtdm == NULL )
	{
#ifdef DEBUG
		printf("malloc wtdm error!\n");
#endif
	}
	wrtrldispmsg *a = ProtclForm.dtfld.wtdm;
	a->CmdGroup = CMDG_WriteRealitmeDisplayMessage;
	a->Cmd = CMD_WriteRealitmeDisplayMessage;
	a->Response = MustResponse;
	//memset(a->Reserved, Fault, sizeof(a->Reserved));
	worldWrite(a->Reserved, 0x002D);
	int deleteareanum = 0x00;				 /* 删除区域个数*/
	a->DeleteAreaNum = deleteareanum;
	int areanum = 0x01; 				   /* 更新区域个数*/
	a->AreaNum = areanum;

	/*存放区域数据长度和区域数据，固定字节2(区域数据长度)+27(区域数据格式)*/
	int Ldtfld_fldtfrm;
	Ldtfld_fldtfrm = sizeof(fielddataframeL2);
	a->AreaLenAndData[0] = (fielddataframeL2 *)malloc(Ldtfld_fldtfrm * areanum);
	if( a->AreaLenAndData == NULL )
	{
#ifdef DEBUG
		printf("malloc fielddataframeL2 error!\n");
#endif
	}
	fielddataframeL2 *b = a->AreaLenAndData[0];
	worldWrite(b->AreaDataLen, Len + 27);
	b->fldtfm.AreaType = 0x00;
	worldWrite(b->fldtfm.AreaX, DspX);
	worldWrite(b->fldtfm.AreaY, DspY);
	worldWrite(b->fldtfm.AreaWidth, DspWdth);
	worldWrite(b->fldtfm.AreaHeight, DspHght);
	b->fldtfm.DynamicAreaLoc = DspAreaNum;		  /* 多个区域同时显示时，需要修改的参数*/
	b->fldtfm.Lines_sizes = 0x00;
	b->fldtfm.RunMode = 0x01;/////0
	//printf("-----------------------\n");
	worldWrite(b->fldtfm.Timeout, 0x0001);
	memset(b->fldtfm.Reserved, Fault, sizeof(b->fldtfm.Reserved));
	b->fldtfm.SingleLine = 0x02;
	b->fldtfm.NewLine = 0x02;
	b->fldtfm.DisplayMode = DspFirm;
	b->fldtfm.ExitMode = 0x00;
	b->fldtfm.Speed = 0x04;
	b->fldtfm.StayTime = 0x07; 
	worldWrite(b->fldtfm.DataLen, Len);
	b->fldtfm.Data = Dispcten;
	//printf("-----------------------\n");
	/*重新存放整理好的数据，用于计算校验值*/
	Ldtfld = Len + 27 + 9;									  /* 数据域长度*/
	uint8 *c = (uint8 *)malloc(sizeof(uint8) * Ldtfld);
	if( c == NULL )
	{
#ifdef DEBUG
		printf("malloc uint8 error!\n");
#endif
	}
	uint8 *begin = c;
	memcpy(c, a, 7);
	c = c + 7;
	memcpy(c, a->AreaLenAndData[0]->AreaDataLen, 2);
	c = c + 2;
	*c = b->fldtfm.AreaType;
	c = c + 1;
	memcpy(c, b->fldtfm.AreaX, 26);
	c = c + 26;
	memcpy(c, b->fldtfm.Data, Len);
	c = begin;
	free(a);
	free(b);

	/*存放包头数据*/
	Ldtpckh = sizeof(datapackethead);					 /* 包头数据长度*/
	worldWrite(ProtclForm.dtpckh.DstAddr, 0x0001);
	worldWrite(ProtclForm.dtpckh.SrcAddr, 0x8000);
	memset(ProtclForm.dtpckh.Reserved, Fault, sizeof(ProtclForm.dtpckh.Reserved));
	ProtclForm.dtpckh.DeviceType = 0xFE;
	ProtclForm.dtpckh.ProtocolVersion = 0x02;
	worldWrite(ProtclForm.dtpckh.DataLen, Ldtfld);

	/*计算校验码并存放*/
	Lchecksum = sizeof(ProtclForm.crc16checksum);	 /* 校验码长度*/
	uint8 *buffer = (uint8 *)malloc(Ldtpckh + Ldtfld);
	if( buffer == NULL )
	{
#ifdef DEBUG
		printf("malloc buffer is error!\n");
#endif
	}
	uint8 *bbegin = buffer;
	memcpy(buffer, ProtclForm.dtpckh.DstAddr, Ldtpckh);
	buffer = buffer + Ldtpckh;
	memcpy(buffer, c, Ldtfld);
	buffer = bbegin;

	uint16 resurcr; 				   /* 校验结果*/
	resurcr = calcCRC(buffer, Ldtpckh + Ldtfld);
	worldWrite(ProtclForm.crc16checksum, resurcr);
	free(buffer);

	uint8 *swtcharbuffer = (uint8 *)malloc(Ldtpckh + Ldtfld + Lchecksum);
	if( swtcharbuffer == NULL)
	{
#ifdef DEBUG
		printf("malloc swtcharbuffer is error!\n");
#endif
	}
	uint8 *swbegin = swtcharbuffer;

	memcpy(swtcharbuffer, ProtclForm.dtpckh.DstAddr, Ldtpckh);
	swtcharbuffer = swtcharbuffer + Ldtpckh;
	memcpy(swtcharbuffer, c, Ldtfld);
	swtcharbuffer = swtcharbuffer + Ldtfld;
	memcpy(swtcharbuffer, ProtclForm.crc16checksum, Lchecksum);
	swtcharbuffer = swbegin;

	/*转义字符处理*/
	uint8  swtcharbuffer1[512];
	switchChar(Ldtpckh + Ldtfld + Lchecksum, swtcharbuffer,swtcharbuffer1);

	/*存放帧头数据*/
	Lfrahd = sizeof(ProtclForm.framehead);			  /* 帧头长度*/
	memset(ProtclForm.framehead, 0xA5, Lfrahd);

	/*存放帧尾数据*/
	Lfratl = sizeof(ProtclForm.frametail);				  /* 帧尾长度*/
	ProtclForm.frametail = 0x5A;

	/*存放待发送的数据*/
	uint8 *wrtbuffer = (uint8 *)malloc(Lfrahd + Lswtnum + Lfratl);
	if( wrtbuffer == NULL )
	{
#ifdef DEBUG
		printf("malloc wrtbuffer is error!\n");
#endif
	}
	uint8 *bbbegin = wrtbuffer;
	memcpy(wrtbuffer, ProtclForm.framehead, Lfrahd);
	wrtbuffer = wrtbuffer + Lfrahd;
	memcpy(wrtbuffer, swtcharbuffer1, Lswtnum);
	wrtbuffer = wrtbuffer + Lswtnum;
	*wrtbuffer = ProtclForm.frametail;
	wrtbuffer = bbbegin;

	free(c);
	free(swtcharbuffer);
#ifdef DEBUG
	int i;
	for( i = 0; i < Lswtnum + Lfratl + Lfrahd; )
	{
		 printf("%02x ", *(wrtbuffer + i));

		 i++;
		 if(i % 10 == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
#endif
	return wrtbuffer;

}
/*******************************************************************************
* 函数名  : check_LedConnect
* 功能	  : 检查和屏的网络连接
* 输入	  : void *SingLedInfo:单个led屏的结构体参数

* 输出	  :
* 返回	  : -1:连接不成功
			0: 连接成功
*******************************************************************************/
int check_LedConnect(led_stru *SingLedInfo)
{
	int j = 0;
	for(;;)
	{
	  if(tcp_blkConn(SingLedInfo->IPAddr,SingLedInfo->COMPort,&(SingLedInfo->LedCon_Fd))<0)
	  {
		  tcp_blkClose(&(SingLedInfo->LedCon_Fd));
		  usleep(500000);
		  j++;
		  if(j < 3)
		  {
			 continue;
		  }
	  }
	  if(j == 3)
	  {
		  //writelog(ZLOG_LEVEL_LED,SingLedInfo->LEDId,"重连三次,连接不上LED屏%d!!请检查IP:%s,PORT:%d!!",SingLedInfo->LEDId,SingLedInfo->IPAddr,SingLedInfo->COMPort);
		  return -1;
	  }
	  return 0;
	}
}
/*******************************************************************************
* 函数名  : wrt2LEDMsgCar
* 功能	  : 发布实时剩余车位数
* 输入	  : 	uint 8 *Dispcten 数字
				void *LedPara led屏结构体

* 输出	  :
* 返回	  : 	-1:发布不成功
				0: 连接成功
*******************************************************************************/
int wrt2LEDMsgCar(uint8 * Dispcten,void *LedPara)
{
	int carnum=0;
	led_stru *Pnode;
	Pnode = (led_stru *)LedPara;
	if(Pnode==NULL||strlen(Dispcten)>4||strlen(Dispcten)==0)
	{
		//writelog(ZLOG_ERR,"-------disply erro!!\n");//
		return -1;
	}
	char showBuf[30];
	char pnBuf[10];
	unsigned char *Wbuf;
	DspAreaNum = 0;  //动态区域号
	DspFirm = 1;	//显示方式，静态显示
	DspHght = 32;	//显示高度，64个像素
	DspWdth = 64;	//显示区域宽度，16个字节 128个像素
	DspX = 0;
	DspY = 0;
    memset(showBuf,0,sizeof(showBuf));
	if(atoi(Dispcten) == 0)
	{
		strcat(showBuf,"\\FO001");//FO001:宋体32*32汉字
		strcat(showBuf,"已满");
	}
	else
	{
		strcat(showBuf,"\\FE001");//FO001:宋体32*32汉字

		switch (strlen(Dispcten))
		{
			case 1:strcat(showBuf,"   ");break;
			case 2:strcat(showBuf,"  ");break;
			case 3:strcat(showBuf," ");break;
		}

		strcat(showBuf,Dispcten);
	}
	
	
	Wbuf = wrtReltDspMsg((uint8 *)showBuf,sizeof(showBuf)-1);
	
	tcpip2Disp_Write(Wbuf,&(Pnode->LedCon_Fd));
	
	usleep(100000);
	free(Wbuf); //释放
	return 0;
}

/*******************************************************************************
* 函数名  : setLED_num
* 功能	  : 发布实时剩余车位数
* 输入	  : 	

* 输出	  : led数据
* 返回	  :	 -1:发布不成功
				0: 连接成功
*******************************************************************************/

int setLED_num(uint8 *ledip,uint32 ledport,uint8 * dispcten)
{
	led_stru ledtempstruct;
	strcpy(ledtempstruct.IPAddr,ledip);
	ledtempstruct.COMPort=ledport;
	//writelog(ZLOG_MNG,"%s:%d  %s\n",ledtempstruct.IPAddr,ledtempstruct.COMPort,dispcten);
	if(check_LedConnect(&ledtempstruct)!=0)//正常连接
	{
		tcp_blkClose(&(ledtempstruct.LedCon_Fd));
		writelog(ZLOG_ERR,"LED connect failed!\n");
		return -1;
	}
	if(wrt2LEDMsgCar(dispcten,&ledtempstruct)!=0)
	{
	    writelog(ZLOG_ERR,"LED display failed!\n");
		tcp_blkClose(&(ledtempstruct.LedCon_Fd));
		return -1;
	}
    writelog(ZLOG_MNG,"LED display success!%s:%d  %s\n",ledtempstruct.IPAddr,ledtempstruct.COMPort,dispcten);
	tcp_blkClose(&(ledtempstruct.LedCon_Fd));
	
	return 0;
}



