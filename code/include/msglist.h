#ifndef __MSGLIST_H__
#define __MSGLIST_H__

#define MAX_MSGLIST_LEN 100				/* 消息队列最大长度 */
#define MAX_MSG_LEN 4096				/* 最大消息长度 */


typedef struct _msg_node_s
{
	char Msg[MAX_MSG_LEN];
	int MsgLen;
	struct _msg_node_s *Next;
}msg_node_s;

typedef struct
{
	msg_node_s *Head;
	msg_node_s *Tail;
	u8 Len;
	pthread_mutex_t Mtx;
	pthread_cond_t Cond;
}msg_list_s;


extern msg_list_s* init_msglist(void);
extern int ins_msglist(msg_list_s *q, char *msg, int msglen);
extern int del_msglist_node(msg_list_s *q);


#endif





