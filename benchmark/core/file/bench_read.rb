require 'benchmark'
require 'benchmark/ips'

small_file  = 'benchmark/core/file/small.txt'
medium_file = 'benchmark/core/file/medium.txt'
large_file  = 'benchmark/core/file/large.txt'

existing_file = "./lib/ffi.rb"

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
