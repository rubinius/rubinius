
`ulimit -v 262144`
`ulimit -s 32768`


file = ARGV.shift
if file
  files = [file]
else
  files = Dir["bm_*.rb"]
end

STDOUT.sync = true

puts("%20s %20s %20s" % ["Test", "MRI", "Rubinius"])
files.each do |file|
  print("%20s " % file)
  start = Time.now
  `ruby time.rb ruby #{file} > #{file}.out.ruby 2>&1`
  if $?.exitstatus == 1
    output = "Error"
  elsif $?.exitstatus == 2
    output = "Timeout"
  else
    output = (Time.now - start).to_s
  end
  
  print("%20s " % output)

  start = Time.now
  `ruby time.rb rbx #{file} > #{file}.out.rbx 2>&1`
  if $?.exitstatus == 1
    output = "Error"
  elsif $?.exitstatus == 2
    output = "Timeout"
  else
    output = (Time.now - start).to_s
  end
  
  puts("%20s " % output)
end
