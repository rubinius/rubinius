#!/bin/zsh

trap '' HUP
trap '' ABRT

[[ -z $1 ]] && exit
unset RUBYOPT      # so that rubinius builds

root=~/continuous
#pastie=$root/bin/pastie.rb
pastie=$root/bin/nopaste
rubinius=$root/repo/rubinius
head=$rubinius/HEAD
thischeckout=$rubinius/$1

cd $head
git pull &>/dev/null

rm -rf $thischeckout &>/dev/null
git clone $head $thischeckout &>/dev/null
cd $thischeckout
git checkout $1 2>/dev/null


rake build &>build_log
# did it build?
if [[ $? -ne 0 ]]; then
	echo "build failed! `cat build_log | $pastie`"
	exit 1
fi
	
# version and stuff
./bin/rbx -v -e 'true' &>ci_log
# a bit of a workaround because ci_log can exec
./bin/ci -f m &>>ci_log

#sleep 5
#old=$(stat -c '%Y' ci_log)
#sleep 10

#while [[ $old != $(stat -c '%Y' ci_log) ]]; do
#	old=$(stat -c '%Y' ci_log)
#	sleep 5
#done

tail -1 ci_log | grep expectations &>/dev/null

# did it pass the spec?
if [[ $? -ne 0 ]]; then
	echo "bin/ci failed! `cat -v ci_log | sed -e 's/\^\[\[0;3[14]m//' -e 's/\^\[\[0m//' | $pastie`"
else
	failures=$(tail -1 ci_log | cut -d' ' -f 5)
	if [[ $failures -gt 0 ]]; then
	       echo "$(tail -1 ci_log); $(cat -v ci_log | sed -e 's/\^\[\[0;3[14]m//' -e 's/\^\[\[0m//' | $pastie)"
        else
	       echo "$(tail -1 ci_log)"
	fi
fi

# save some space, until it gets cleaned.
#rake distclean &>/dev/null
#rm -rf .git &>/dev/null

exit 0
