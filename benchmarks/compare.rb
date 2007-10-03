file1 = File.open(ARGV[0])
file2 = File.open(ARGV[1])

while true
  mri_line = file1.gets
  rbx_line = file2.gets
  unless mri_line && rbx_line
    break
  else
    next unless mri_line.match(/\(\s*([\d\.]*)\)/)
    mri_time = mri_line.match(/\(\s*([\d\.]*)\)/)[1].to_f
    rbx_time = rbx_line.match(/\(\s*([\d\.]*)\)/)[1].to_f
    title = mri_line.match(/^([^ ] ?)*/)[0]
    $stdout.puts "#{title}:"
    $stdout.puts "  MRI: #{mri_time}"
    $stdout.puts "  RBX: #{rbx_time}"
    $stdout.puts "  ratio: #{"%0.4f" % (mri_time / rbx_time)}"
    $stdout.puts
  end
end
