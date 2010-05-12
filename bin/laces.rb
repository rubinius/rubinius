target = "bin/rbx"

STDOUT.sync = true

Dir["spec/laces/*.rb"].each do |file|
  lines = File.readlines(file)

  line = lines.shift
  while line !~ /\s*#\s*OUTPUT.*/
    line = lines.shift
  end

  output = lines.map do |line|
    sub = line.gsub!(/\s*#\s*/, "")
    sub.strip! if sub
    sub
  end.flatten.join("\n").strip

  print "#{file}: "
  actual = `#{target} #{file}`.strip

  if actual == output
    puts "OK"
  else
    puts "FAILED"
    p output
    p actual
  end
end
