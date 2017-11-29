#include "include.h"

/*----------------------------------------------------
* 函数名  : uint2ubuf
* 功能	  : 无符号正想转换，避免大小端
* 输入    : @data : 无符号整形入参
		  
* 输出	  : @buf : 转成结果出参，注意，高位在前buf[0]
* 返回	  : 0: 正常
			<0:异常
----------------------------------------------------*/
void uint2ubuf(unsigned int data,unsigned char *buf)
{
	if (buf == NULL)
	{
		writelog(ZLOG_ERR,"[uint2ubuf]:Invalid argument!");
		return;
	}

	*buf=(data&0xff000000)>>24;
	*(buf+1)=(data&0x00ff0000)>>16;
	*(buf+2)=(data&0x0000ff00)>>8;
	*(buf+3)=data&0x000000ff;
}
/*----------------------------------------------------
* 函数名  : ubuf2uint
* 功能	  : 无符号正想转换，避免大小端
* 输入    : @buf : 转成结果出参，注意，高位在前buf[0]
		  
* 输出	  : @data : 无符号整形入参
* 返回	  : 0: 正常
			<0:异常
----------------------------------------------------*/
void ubuf2uint(unsigned char *buf,unsigned int *data)
{
	if (buf == NULL || data == NULL)
	{
		writelog(ZLOG_ERR,"[ubuf2uint]:Invalid argument!");
		return;
	}
	*data=((*buf)<<24 | \
			(*(buf+1))<<16 | \
			(*(buf+2))<<8 | \
			(*(buf+3)));
}


/*----------------------------------------------------
* 函数名  : tcp_blkClose
* 功能	  : 连接服务器
* 输入    : @fd : 连接句柄，指针类型

* 输出	  : 无
* 返回	  : 0: 正常
			<0:异常
----------------------------------------------------*/
int tcp_blkClose(int *fd)
{
	if(fd == NULL)
	{
		writelog(ZLOG_GW,"[tcp_blkClose]:Invalid argument!");
		return -1;		
	}
	return(close(*fd));
}


/*----------------------------------------------------
* 函数名  : tcp_blkConn
* 功能	  : 阻塞连接tcp
* 输入    : @IP : 文件名
		  : @Port: 解析结果
		  : @fd 连接句柄，指针类型
		  
* 输出	  : 无
* 返回	  : 0: 正常
			<0:异常
----------------------------------------------------*/
int tcp_blkConn(char *ip,unsigned int port,int *fd)
{
	if((ip == NULL) || (port == 0) || (fd == NULL))
	{
		writelog(ZLOG_ERR,"[tcp_blkConn]:Invalid argument!");
		return -1;		
	}
	struct sockaddr_in client_addr;
	memset(&client_addr,0,sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);
	client_addr.sin_port = htons(0);

	*fd = socket(AF_INET,SOCK_STREAM,0);
	if( *fd < 0)
	{
		writelog(ZLOG_ERR,"[tcp_blkConn]:Create to %s socket error!",ip);
		tcp_blkClose(fd);
		return -1;
	}
	/* block communition recv and send over time setting */
	struct timeval t;
	t.tv_sec = BLOCK_SEC;
	t.tv_usec = BLOCK_MSEC*1000;
	setsockopt(*fd, SOL_SOCKET, SO_SNDTIMEO , &t, sizeof(t));
	setsockopt(*fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));

	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;

	if( bind(*fd,(struct sockaddr*)&client_addr,sizeof(client_addr)))
	{
		writelog(ZLOG_ERR,"[tcp_blkConn]Create to %s bind error",ip);
		tcp_blkClose(fd);
		return -1;
	}
	
	if(inet_aton(ip,&server_addr.sin_addr) == 0)
	{
		writelog(ZLOG_ERR,"[tcp_blkConn]Create to %s IP error",ip);
		tcp_blkClose(fd);
		return -1;
	}
	
	server_addr.sin_port = htons(port);

	socklen_t server_addr_length = sizeof(server_addr);

	if(connect(*fd,(struct sockaddr*)&server_addr, server_addr_length) < 0)
	{
		writelog(ZLOG_ERR,"[tcp_blkConn]Connect to %s:%d faile!",ip,port);
		tcp_blkClose(fd);
		return -1;
	}
	writelog(ZLOG_SYS,"[tcp_blkConn]Connect to %s:%d success!",ip,port);
	return 0;

}

/************************************************************************************
* GetLocalNetInfo
* DESCRIPTION	: 修改本地的网口号对应的网路地址和子网掩码地址
* @ *ethno		: 网口号(入口),可以是eth0,eth1
* @ *ip		: 新IP地址(出口)
* @ *netmask	: 新子网掩码(出口)
* @ Return		: 正确返回0，其它为-1
* ----
*************************************************************************************/
int get_localNetInfo(char *ethno, char *ip, char *netmask)
{

	int sock_get_ip;

	struct	 sockaddr_in *sin;
	struct	 ifreq ifr_ip;

	if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		 writelog(ZLOG_SYS,"socket create failse...GetLocalIp");
//		 close( sock_get_ip );//_by_pbh_2015-01-26 注释fclose/close关闭空的句柄
		 return -1;
	}

   // memset(&ifr_ip, 0, sizeof(ifr_ip));
   // strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
	bzero(&ifr_ip, sizeof(struct ifreq));
	strcpy(ifr_ip.ifr_name, ethno);
	if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
	{
		 close( sock_get_ip );
		 return -1;
	}
	sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
	strcpy(ip,inet_ntoa(sin->sin_addr));

	writelog(ZLOG_SYS, "Get local ip:%s",ip);

	bzero(&ifr_ip, sizeof(struct ifreq));
	strcpy(ifr_ip.ifr_name, ethno);
	if( ioctl( sock_get_ip, SIOCGIFNETMASK, &ifr_ip) < 0 )
	{
		 close( sock_get_ip );
		 return -1;
	}
	sin = (struct sockaddr_in *)&ifr_ip.ifr_netmask;
	strcpy(netmask,inet_ntoa(sin->sin_addr));

	writelog(ZLOG_SYS, "Get local netmask:%s",netmask);
	close( sock_get_ip );
	return 0;
}

/************************************************************************************
* SetLocalNetInfo
* DESCRIPTION	: 修改本地的网口号对应的网路地址和子网掩码地址
* @ *ethno		: 网口号
* @ *ip		: 新IP地址
* @ *netmask	: 新子网掩码
* @ Return		: 正确返回0，其它为-1
* ----
*************************************************************************************/
int set_LocalNetInfo( char *ethno, char *ip, char *netmask, char *gateway)
{
	struct ifreq req;
	struct sockaddr_in* host = NULL;
	struct rtentry rt;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( -1 == sockfd )
	{
		writelog(ZLOG_ERR,"[Set_LocalNetInfo]error:socket create failse...\r\n");
//		close(sockfd);//_by_pbh_2015-01-26 注释fclose/close关闭空的句柄
		return -1;
	}

	bzero(&req, sizeof(struct ifreq));
	strcpy(req.ifr_name, ethno);
	host = (struct sockaddr_in*)&req.ifr_addr;
	host->sin_family = AF_INET;
	if ( 1 != inet_pton(AF_INET, ip, &(host->sin_addr)) )
	{
		writelog(ZLOG_ERR,"[Set_LocalNetInfo]error:inet_pton failse...\r\n");
		close(sockfd);
		return -1;
	}
	if ( ioctl(sockfd, SIOCSIFADDR, &req) < 0 )
	{
		writelog(ZLOG_ERR,"[Set_LocalNetInfo]error:SIOCSIFADDR failse...\r\n");
		close(sockfd);
		return -1;
	}

	bzero(&req, sizeof(struct ifreq));
	strcpy(req.ifr_name, ethno);
	host = (struct sockaddr_in*)&req.ifr_addr;
	host->sin_family = AF_INET;
	if ( 1 != inet_pton(AF_INET, netmask, &(host->sin_addr)) )
	{
		writelog(ZLOG_ERR,"[Set_LocalNetInfo]error:inet_pton failse...\r\n");
		close(sockfd);
		return -1;
	}
	if ( ioctl(sockfd, SIOCSIFNETMASK, &req) < 0 )
	{
		writelog(ZLOG_ERR,"[Set_LocalNetInfo]error:SIOCSIFNETMASK failse...\r\n");
		close(sockfd);
		return -1;
	}

	if( strcmp(gateway, "") != 0 )
	{
		#if 0
		char buf[128] = {0};

		strcat(buf, "route add default gw ");
		strcat(buf, gateway);
		system(buf);
		#endif

		#if 1
		memset(&rt, 0, sizeof(struct rtentry));
		memset(host, 0, sizeof(struct sockaddr_in));
		host->sin_family = AF_INET;
		host->sin_port = 0;
		if( inet_aton(gateway, &host->sin_addr) < 0 )
		{
			writelog(ZLOG_ERR,"[Set_LocalNetInfo]inet_aton error\r\n");
		}
		memcpy ( &rt.rt_gateway, host, sizeof(struct sockaddr_in));
		((struct sockaddr_in *)&rt.rt_dst)->sin_family = AF_INET;
		((struct sockaddr_in *)&rt.rt_genmask)->sin_family = AF_INET;
		rt.rt_flags = RTF_GATEWAY;
		if( ioctl(sockfd, SIOCADDRT, &rt) < 0 )
		{
			writelog(ZLOG_ERR,"[Set_LocalNetInfo]ioctl(SIOCADDRT) error in set_default_route\r\n");
			close(sockfd);
			return -1;
		}
		#endif
	}
	close(sockfd);
	writelog(ZLOG_SYS,"[Set_LocalNetInfo] success!");
	return 0;
}


