#!/bin/sh

find . -type f                                                                             >  contribution_files.txt
perl -p -e '$_ = substr($_,2)'                                      contribution_files.txt >  contribution_files_stripped.txt
egrep -v '^([.]git|tmp|stdlib)'                            contribution_files_stripped.txt >  contribution_files_limited_1.txt
egrep -v '[.](pdf|graffle|rb(a|c)|o|dylib|gen)$'          contribution_files_limited_1.txt >  contribution_files_limited_2.txt
egrep -v '/[.]|/external_libs/|test/mri/|benchmark/yarv'  contribution_files_limited_2.txt >  contribution_files_limited_3.txt
cat contribution_files_limited_3.txt | xargs -n 1 git blame                                >> contribution_blames.txt
egrep --binary-files=text -v 'Not Committed Yet'                   contribution_blames.txt >  contribution_blames_limited.txt
perl -p -e 's/^[^(]+\(([^0-9]+).+/$1/'                     contribution_blames_limited.txt >  contribution_names_white.txt
perl -p -e 's/\s*$/\n/'                                       contribution_names_white.txt >  contribution_names.txt
sort contribution_names.txt | uniq -c > contribution_counts.txt
cat contribution_counts.txt | bin/contributors.rb
rm contribution_*.txt
