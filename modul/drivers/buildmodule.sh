#########################################################################
#	File Name: buildmodule.sh
#	Author   : fengjunhui
#	Email    : 18883765905@163.com
#	Created  : 2018年09月11日 星期二 16时14分33秒
#########################################################################
#!/bin/bash

echo "*****************************************************"
echo "just to compile the module int the current directory!"
echo "*****************************************************"

echo $PWD

for file in $PWD/*
do
	if [ -d "$file" ]
	then
		echo "$file is directory"
		cd $file 
		make clean
		make
		cp *.ko  *_test ../
		make clean
		cd -
	elif [ -f "$file" ]
	then
		echo "$file is file"
	fi
done

echo "*******************************************************"
echo "***********  compile the module over! *****************"
echo "*Do you want to mv to the file system directory y/n?*"
echo "*******************************************************"

read yourchoice

case $yourchoice in
	"y"|"yes"|"Y"|"YES")
		echo "moving,wait a moment,please"
		sleep 1
		sudo mv *.ko *_test ~/source/rootfs/drv/ 
		echo "moved,you are lazy,haha!"
		;;
	"n"|"no"|"N"|"NO")
		echo "maybe you need to copy it by hand."
		;;
	*)
		echo "I get $var,maybe your input is wrong !"
		echo "There is no chance,you need to do it manually !"
		;;
esac
# --- end of case ---""""""""""





