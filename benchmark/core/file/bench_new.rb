require 'benchmark'
require 'benchmark/ips'

existing_file = "./lib/ffi.rb"

Benchmark.ips do |x|
  x.report "File.new" do
    File.new(existing_file)
  end
end
