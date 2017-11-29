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
	if [ $? -eq 0 ]; then #如果下载成功	
		echo "tftp transmission OK"
		break
	else
		echo "tftp transmission failed! Please check and try it once more!"
		echo ""
	fi
done

##解压升级文件包到临时目录下,并重新命名，备份
if [ -d "$tmpPath" ]; then 
	echo "升级包备份目录已存在" 
else
	mkdir tmp
fi 
rm -rf $tmpPath$prjName.tmp
tar -xvf $prjName.tar -C $tmpPath
mv $tmpPath$prjName $tmpPath$prjName.tmp
echo "升级包备份成功"
##以下为升级过程
##非首次升级
if [ -d "$usrPath$prjName" ]; then 
	##非首次升级，kill原先的进程
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
	##非首次升级，创建新的升级临时文件	
	mkdir $usrPath$prjName.tmp
	tar -xvf $prjName.tar -C $usrPath$prjName.tmp

	i=0
	len=${#appDirUpdate[*]}
	##非首次升级，拷贝新的启动脚本到etc
	cp $usrPath$prjName.tmp/$prjName/etc/S91euds  $usrPath$prjName/etc/
	
	##非首次升级，拷贝新的bin demo lib
	while [ $i -lt $len ]
	do
		rm -rf $usrPath$prjName/${appDirUpdate[$i]}
		echo "rm -rf $usrPath$prjName/${appDirUpdate[$i]}"
		
		mv $usrPath$prjName.tmp/$prjName/${appDirUpdate[$i]} $usrPath$prjName
		echo "mv $usrPath$prjName.tmp/$prjName/${appDirSave[$i]} $usrPath$prjName"
		
		let i++
	done
	##非首次升级，删除旧的临时升级文件包
	rm -rf $usrPath$prjName.tmp 
else
	##首次升级,直接解压
	echo "-----------------------------"
	echo "This is the first time Update!!"
	tar -xvf $prjName.tar -C $usrPath
fi
 
##升级,删除旧的系统bin目录下的应用程序
i=0
len=${#usrApp[*]}
while [ $i -lt $len ]
do
	wr rm -rf /usr/bin/${usrApp[$i]}*
	echo "rm -rf /usr/bin/${usrApp[$i]}*"
	let i++
done
##升级,删除升级文件压缩包
rm -rf $prjName.tar
##升级,删除旧的启动脚本和停车场的启动脚本
wr rm -rf /etc/init.d/S91ult
wr rm -rf /etc/init.d/S91park
wr rm -rf /etc/init.d/S91euds
##升级,拷贝新的启动脚本 lib文件 bin文件到系统目录
wr cp $usrPath$prjName/etc/S91euds /etc/init.d/
wr cp $usrPath$prjName/lib/*.so* /usr/lib/
wr cp $usrPath$prjName/bin/* /usr/bin/

date
echo "------Update success!!--------"
echo "-----Now it going to reboot---"

sleep 1

reboot
