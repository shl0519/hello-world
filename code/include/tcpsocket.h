#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

#define BLOCK_SEC 3    /* ����ʱ���� */
#define BLOCK_MSEC 500 /* ����ʱ����� */
#define NET_TIME_OUT 1 /* ��ʱʱ������ */


extern void uint2ubuf(unsigned int data,unsigned char *buf);
extern void ubuf2uint(unsigned char *buf,unsigned int *data);
extern int tcp_blkConn(char *ip,unsigned int port,int *fd);
extern int tcp_blkClose(int *fd);
extern int get_LocalNetInfo(char *ethno, char *ip, char *netmask);
extern int set_LocalNetInfo( char *ethno, char *ip, char *netmask, char *gateway);

#endif

