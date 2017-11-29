#ifndef __ZMQ_COMMU_H__
#define __ZMQ_COMMU_H__


#define STR_RECV_FILE_PATH "/opt/euds/tmp/"  


#define ZMQ_RECV_OVT  1000   // millsecond
#define ZMQ_SEND_OVT  1000   // millsecond
#define ZMQ_MAX_MSG_LEN 4096

extern int zmqsend_msg(char *destip,int destport,char *msg,int len);
extern int getfilesvr_init(int *svrport);
extern int getmsgsvr_init(int *svrport);
extern int fmt2str_msg(char *inbuf,int inlen,char *outbuf);
#endif

