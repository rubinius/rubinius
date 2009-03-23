Bench.run [100] do |n|
  n.times do
    count = 0
    fname = File.dirname(__FILE__) + "/random.input"
    File.open(fname, "r").each_line do |line|
      count += line.to_i
    end
    puts count
  end
end
