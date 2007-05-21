#!/bin/bash
TIME="%e"
CMD1=ruby
CMD2=rbx
ulimit -v 262144
ulimit -s 32768

FILES=bm_*.rb
if [ "$1x" != "x" ]; then
 FILES=$1
fi

echo "Test	$CMD1	$CMD2"
for FILE in $FILES
do
	BASE=`echo $FILE | awk -F '.' '{ print $1 }'`
	RUBYTIME=`/usr/bin/time -f "$TIME" sh -c "ruby $FILE > $FILE.out.ruby" 2>&1`
	if [ $? != 0 ]; then
	 RUBYTIME='Error'
	fi
	RBXTIME=`/usr/bin/time -f "$TIME" sh -c "rbx $FILE > $FILE.out.rbx" 2>&1`
	if [ $? != 0 ]; then
	 RBXTIME='Error'
	fi
	echo "$BASE	$RUBYTIME	$RBXTIME"
done
