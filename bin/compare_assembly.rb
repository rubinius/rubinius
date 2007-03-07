require 'rubygems'
require 'tempfile'

file = ARGV.shift

t1 = Tempfile.new("compare")
t2 = Tempfile.new("compare2")

t1.close
t2.close

r18 = `ruby -Ilib bin/rasm.rb r18 #{file} > #{t1.path}`
puts "=> r18"
rbx = `./shotgun/rubinius bin/rasm.rb rubinius #{file} > #{t2.path}`
puts "=> rubinius"

system("diff -u #{t1.path} #{t2.path}")
p $?.exitstatus
