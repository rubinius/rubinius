#!/bin/bash
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
	RUBYTIME=`./time.rb sh -c "ruby $FILE > $FILE.out.ruby" 2>&1`
	if [ $? == 1 ]; then
	 RUBYTIME='Error'
	fi
	if [ $? == 2 ]; then
	 RUBYTIME='Timeout'
	fi
	RBXTIME=`./time.rb sh -c "rbx $FILE > $FILE.out.rbx" 2>&1`
	if [ $? == 1 ]; then
	 RBXTIME='Error'
	fi
	if [ $? == 2 ]; then
	 RBXTIME='Timeout'
	fi
	echo "$BASE	$RUBYTIME	$RBXTIME"
done
