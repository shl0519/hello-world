#!/bin/sh

tftpSerIP=''
tmpPath=/opt/tmp/
usrPath=/opt/
prjName=euds

usrApp=(redoeuds.sh \
        EUDS- \
        )
        
appDirUpdate=(bin demo lib)

i=0
len=${#usrApp[*]}

#echo ${usrApp[@]}
#echo $len

while true
do  
	echo -n "Please Input tftp server IP Address:"
	read tftpSerIP
	##echo $tftpSerIP
	cd $usrPath
	tftp -gr $prjName.tar $tftpSerIP      
	if [ $? -eq 0 ]; then #������سɹ�	
		echo "tftp transmission OK"
		break
	else
		echo "tftp transmission failed! Please check and try it once more!"
		echo ""
	fi
done

##��ѹ�����ļ�������ʱĿ¼��,����������������
if [ -d "$tmpPath" ]; then 
	echo "����������Ŀ¼�Ѵ���" 
else
	mkdir tmp
fi 
rm -rf $tmpPath$prjName.tmp
tar -xvf $prjName.tar -C $tmpPath
mv $tmpPath$prjName $tmpPath$prjName.tmp
echo "���������ݳɹ�"
##����Ϊ��������
##���״�����
if [ -d "$usrPath$prjName" ]; then 
	##���״�������killԭ�ȵĽ���
	echo "-----------------------------"
	echo "This isn't the first time Update!!"
	
	while [ $i -lt $len ]
	do
		#echo "killall -9 ${usrApp[$i]}"
		#killall -9 ${usrApp[$i]}

		echo "kill ${usrApp[$i]}"
		kill -s 9 $(ps | grep ${usrApp[$i]} | awk 'NR==1' | awk '{print $1}') 
		
		let i++
	done
	##���״������������µ�������ʱ�ļ�	
	mkdir $usrPath$prjName.tmp
	tar -xvf $prjName.tar -C $usrPath$prjName.tmp

	i=0
	len=${#appDirUpdate[*]}
	##���״������������µ������ű���etc
	cp $usrPath$prjName.tmp/$prjName/etc/S91euds  $usrPath$prjName/etc/
	
	##���״������������µ�bin demo lib
	while [ $i -lt $len ]
	do
		rm -rf $usrPath$prjName/${appDirUpdate[$i]}
		echo "rm -rf $usrPath$prjName/${appDirUpdate[$i]}"
		
		mv $usrPath$prjName.tmp/$prjName/${appDirUpdate[$i]} $usrPath$prjName
		echo "mv $usrPath$prjName.tmp/$prjName/${appDirSave[$i]} $usrPath$prjName"
		
		let i++
	done
	##���״�������ɾ���ɵ���ʱ�����ļ���
	rm -rf $usrPath$prjName.tmp 
else
	##�״�����,ֱ�ӽ�ѹ
	echo "-----------------------------"
	echo "This is the first time Update!!"
	tar -xvf $prjName.tar -C $usrPath
fi
 
##����,ɾ���ɵ�ϵͳbinĿ¼�µ�Ӧ�ó���
i=0
len=${#usrApp[*]}
while [ $i -lt $len ]
do
	wr rm -rf /usr/bin/${usrApp[$i]}*
	echo "rm -rf /usr/bin/${usrApp[$i]}*"
	let i++
done
##����,ɾ�������ļ�ѹ����
rm -rf $prjName.tar
##����,ɾ���ɵ������ű���ͣ�����������ű�
wr rm -rf /etc/init.d/S91ult
wr rm -rf /etc/init.d/S91park
wr rm -rf /etc/init.d/S91euds
##����,�����µ������ű� lib�ļ� bin�ļ���ϵͳĿ¼
wr cp $usrPath$prjName/etc/S91euds /etc/init.d/
wr cp $usrPath$prjName/lib/*.so* /usr/lib/
wr cp $usrPath$prjName/bin/* /usr/bin/

date
echo "------Update success!!--------"
echo "-----Now it going to reboot---"

sleep 1

reboot
