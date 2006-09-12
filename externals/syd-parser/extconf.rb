require 'rbconfig'
require 'mkmf'

create_makefile('sydney_parser')


File.open("Makefile","a+") do |f|
    f << <<END
sydparse.c: sydparse.y
	bison -o sydparse.c sydparse.y
END
end
