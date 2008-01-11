#!/usr/bin/env ruby
type1 = ARGV[0][1..-1]
file1 = File.open(ARGV[1])
type2 = ARGV[2][1..-1]
file2 = File.open(ARGV[3])

rehearsal = true

while true
  mri_line = file1.gets
  rbx_line = file2.gets
  if rehearsal == true
    rehearsal = false if mri_line =~ /\- total/
    next
  end
  unless mri_line && rbx_line
    break
  else
    next unless mri_line.match(/\(\s*([\d\.]*)\)/)
    mri_time = mri_line.match(/\(\s*([\d\.]*)\)/)[1].to_f
    rbx_time = rbx_line.match(/\(\s*([\d\.]*)\)/)[1].to_f
    title = mri_line.match(/^([^ ] ?)*/)[0]
    $stdout.puts "#{title}:"
    $stdout.puts "  #{type1}: #{mri_time}"
    $stdout.puts "  #{type2}: #{rbx_time}"
    $stdout.puts "  ratio: #{"%0.4f" % (rbx_time / mri_time)}"
    $stdout.puts
  end
end
