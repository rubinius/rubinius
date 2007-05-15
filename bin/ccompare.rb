file = ARGV.shift
system "bin/obsolete.rcompile #{file} > /dev/null"
sum = `md5 #{file}c`.split(/\s+/).last
system "./shotgun/rubinius compile #{file} > /dev/null"
sum2 = `md5 #{file}c`.split(/\s+/).last

if sum != sum2
  puts "#{file} Compiled differently!!!"
  exit 1
end

puts "#{file}: OK"
exit 0
