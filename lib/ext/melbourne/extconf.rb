require 'rbconfig'
require 'mkmf'

create_makefile('melbourne')

File.open("Makefile","a+") do |f|
  f.puts <<END
grammar.cpp: grammar.y
	bison -o grammar.cpp grammar.y
END
end
