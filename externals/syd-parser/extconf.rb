require 'rbconfig'
require 'mkmf'

if ENV['DEBUG'] == '1'
  with_cflags("-g") do
    create_makefile('sydney_parser')
  end 
else
  create_makefile('sydney_parser')
end

File.open("Makefile","a+") do |f|
    f << <<END
sydparse.c: sydparse.y
	bison -o sydparse.c sydparse.y
END
end
