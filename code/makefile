
_MY_VERSION := EUDS-0.02.03_XXG
_MY_CLASS := EUDS
_MY_PACKAGE := euds.tar

_APP_TOOL_DIR := ../tools

_APP_INCLUDE_DIR := -I./include  -I./include/common  -I./include/sqlite3  -I./include/zlog -I./include/zmq

_APP_SRC_DIR := ./src
_APP_TARGET_DIR := ../target
_APP_SYS_DIR := ../euds
_APP_BIN_OBJS :=  $(_APP_SRC_DIR)/main.o 			$(_APP_SRC_DIR)/zlog_usr.o   	$(_APP_SRC_DIR)/crc32.o 	\
				  $(_APP_SRC_DIR)/tcpsocket.o  		$(_APP_SRC_DIR)/gw.o 			$(_APP_SRC_DIR)/gwlist.o  	\
				  $(_APP_SRC_DIR)/sqlite3_usr.o  	$(_APP_SRC_DIR)/thre.o 			$(_APP_SRC_DIR)/thre_usr.o	\
				  $(_APP_SRC_DIR)/syscfg.o  		$(_APP_SRC_DIR)/zmq_commu.o 	$(_APP_SRC_DIR)/msglist.o   \
				  $(_APP_SRC_DIR)/msgproc.o			$(_APP_SRC_DIR)/led.o			$(_APP_SRC_DIR)/m2l.o		\
				  $(_APP_SRC_DIR)/monitor.o			$(_APP_SRC_DIR)/gpio.o	   		$(_APP_SRC_DIR)/gpio_usr.o	
				  
CC = /opt/arm-2011.03/bin/arm-none-linux-gnueabi-gcc
_APP_LIB_DIR := -L../lib/

#_APP_LIB_DIR = -L /home/smbshare/nfs/park/euds/EUDS-0.02.01/lib


LIBS := -lpthread -ldl -lm -lrt -lcrypt $(_APP_LIB_DIR) -lsqlite3 -lzlog -lzmq
CFLAGS += -Wall -O2 $(_APP_INCLUDE_DIR)
#CFLAGS +=  -O2 $(_APP_INCLUDE_DIR)

.PHONY : all
all : clean APP
		
APP : $(_APP_BIN_OBJS)
	-rm -rf $(_APP_SYS_DIR)/bin/$(_MY_CLASS)*
	-mkdir -p $(_APP_TARGET_DIR)/$(_MY_VERSION)
	$(CC) $(LIBS) $(_APP_BIN_OBJS) $(CFLAGS) -o $(_APP_SYS_DIR)/bin/$(_MY_VERSION)
	-cp $(_APP_TOOL_DIR)/*  $(_APP_TARGET_DIR)/$(_MY_VERSION)
	-tar  -cvf  $(_APP_TARGET_DIR)/$(_MY_VERSION)/$(_MY_PACKAGE)  $(_APP_SYS_DIR)/*
	@echo build $(TARGET) success!
	@echo $(shell date)

##$(_APP_SRC_DIR)/main.o : $(_APP_SRC_DIR)/main.c
##	$(CC) $(CFLAGS) -c $< -o $@
##$(_APP_SRC_DIR)/zlog_usr.o : $(_APP_SRC_DIR)/zlog_usr.c
##	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	-rm -rf $(_APP_SRC_DIR)/*.o 
	-rm -rf $(_APP_TARGET_DIR)/$(_MY_VERSION)
	-rm -rf $(_APP_SYS_DIR)/bin/$(_MY_VERSION)
	@echo clean success!
	@echo $(shell date)
