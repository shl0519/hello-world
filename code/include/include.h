#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <time.h>
#include <signal.h>
#include <dlfcn.h>
#include <dirent.h>
#include </usr/include/net/if.h>
#include <net/route.h>
#include <unistd.h>
#include <sys/reboot.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>


#include "typedef.h"
#include "zlog.h"
#include "zlog_usr.h"

#include "tcpsocket.h"
#include "crc32.h"
#include "gwlist.h"
#include "gw.h"

#include "sqlite3.h"
#include "sqlite3_usr.h"

#include "gpio.h"
#include "gpio_usr.h"

#include "syscfg.h"



#include "zmq.h"
#include "zhelpers.h"
#include "zmq_commu.h"

#include "thre.h"
#include "thre_usr.h"

#include "msglist.h"
#include "msgproc.h"
#include "led.h"
#include "m2l.h"

#include "monitor.h"







#define gettid() syscall(__NR_gettid)//_by_pbh_2015-01-20增加线程tid号的打印
#define SENDOT 1


#define MAX_ALLOW_CPU_USR 95
#define MAX_ALLOW_MEM_USR 90


#endif



