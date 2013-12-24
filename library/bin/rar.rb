#!/usr/bin/env rubinius

require 'bytecode/compiler'
require 'bytecode/rubinius'
require 'codearchive'

until ARGV.empty?
  cmd = ARGV.shift
  
  case cmd
  when "-d"
    Dir.chdir(ARGV.shift)
        
  when "-l"
    arch = ARGV.shift
    puts "Files in #{arch}:"
    ca = CodeArchive.new(arch)
    ca.files.each do |file|
      print "  "
      puts file
    end
    
  when "-u"
    dir = ARGV.shift
    arch = ARGV.shift

    ca = CodeArchive.new(arch)
    cnt = 0
    ca.update_from_directory(dir) do |file|
      cnt += 1
      puts "Compiling and refreshing #{file}..."
    end
    
    puts "Updated #{cnt} files."
  when "-t"
    dir = ARGV.shift
    arch = ARGV.shift

    ca = CodeArchive.new(arch)
    puts "Files out of date in archive:"
    ca.find_out_of_date(dir).each do |file|
      print "  "
      puts file
    end
  else
    puts "Unknown command '#{cmd}'"
    exit 1
  end
end
