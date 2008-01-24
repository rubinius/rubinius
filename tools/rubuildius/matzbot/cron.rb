#!/usr/bin/env ruby

matzdir = "~/matzbot/"

if `ps -A | grep \`cat #{matzdir}matzbot.pid\` | wc`.to_i.zero?
  system("cd #{matzdir}; rm matzbot.pid; ./launch.rb -d -w `cat the_password`")
end