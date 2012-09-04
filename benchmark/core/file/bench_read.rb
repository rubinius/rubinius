require 'benchmark'
require 'benchmark/ips'

small_file  = File.join(File.dirname(__FILE__), 'small.txt')
medium_file = File.join(File.dirname(__FILE__), 'medium.txt')
large_file  = File.join(File.dirname(__FILE__), 'large.txt')

Benchmark.ips do |x|
  x.report "File.read a small file" do
    File.read(small_file)
  end

  x.report "File.read a medium file" do
    File.read(medium_file)
  end

  x.report "File.read a large file" do
    File.read(large_file)
  end

end
