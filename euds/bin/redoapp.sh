#!/bin/sh

if [ $# != 1 ]; then 
	echo "USAGE: $0 euds_version_number" 
	exit 1; 
fi
#����汾��
eudsver="$1"

#�����汾�����¼��־����Ŀ¼		
dir=/opt/euds/log/intraday/

#�������ʱ����
speed=5

#���ִ����
while true
	do
		ps aux | grep $eudsver | grep -v grep | grep -v $0
		if [ $? -ne 0 ] #������̲����ھ�������
		then	
			dateV=`date -I`
			FileName=`ls $dir$dateV* | grep _sys.txt | head -n 1`
			Current_Time=`date "+%Y-%m-%d %H:%M:%S "`
			echo -e "$Current_Time    [redoeuds.sh:]    redo $eudsver\\r" >> $FileName	
			
			/opt/euds/bin/$eudsver			
		else #������̴�����˯��			
			sleep $speed			
		fi	
	done
