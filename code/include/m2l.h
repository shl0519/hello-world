#ifndef __M2L_H__
#define __M2L_H__

#define M2L_CMD_TOTAL 4

#define REBOOT 0xE1
#define TIMING 0xE2
#define GET_SPACE_TOTAL 0xE3
#define GET_SPACE_DETAIL 0xE4


#define EXEC_SUCCESS 100
#define EXEC_FAILE 101
#define FRAME_TAIL_ERR 2
#define FRAME_HEAD_ERR 1
#define FRAME_TAIL_ERR 2
#define FRAME_CMD_ERR 4

extern int m2lmsg_anlys(char *buf,int len,char *content,int *res,int *cmd);
extern int fmtrly_m2lmsg(char *inbuf,int cmd,int execres,char *outbuf,int *outlen);
extern int exec_m2lmsg(char *content,int cmd,char *res);

#endif







