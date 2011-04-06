require 'benchmark'
require 'benchmark/ips'

existing_file = "./lib/ffi.rb"

Benchmark.ips do |x|
  x.report "File.stat" do
    File.stat(existing_file)
  end
end
