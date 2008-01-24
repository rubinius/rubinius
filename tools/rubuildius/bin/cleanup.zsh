#!/bin/zsh

setopt extendedglob

cd $1

integer ndirs
dirs=(*~HEAD(/N))
(( ndirs = ${#dirs} ))

# if there are more than 20 dirs, delete the last 10 of them.
# be sure to never delete HEAD accidently.
[[ $ndirs -gt 20 ]] && rm -rf *~HEAD(D/om[-11,-1])

exit 0
