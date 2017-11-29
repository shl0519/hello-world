#!/bin/sh

if [ $# != 1 ]; then 
	echo "USAGE: $0 euds_version_number" 
	exit 1; 
fi
#程序版本号
eudsver="$1"

#重启版本程序记录日志所在目录		
dir=/opt/euds/log/intraday/

#检测程序的时间间隔
speed=5

#检测执行体
while true
	do
		ps aux | grep $eudsver | grep -v grep | grep -v $0
		if [ $? -ne 0 ] #如果进程不存在就重启它
		then	
			dateV=`date -I`
			FileName=`ls $dir$dateV* | grep _sys.txt | head -n 1`
			Current_Time=`date "+%Y-%m-%d %H:%M:%S "`
			echo -e "$Current_Time    [redoeuds.sh:]    redo $eudsver\\r" >> $FileName	
			
			/opt/euds/bin/$eudsver			
		else #如果进程存在则睡眠			
			sleep $speed			
		fi	
	done
