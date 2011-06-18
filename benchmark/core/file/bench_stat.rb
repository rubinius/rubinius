require 'benchmark'
require 'benchmark/ips'

existing_file = File.join(__FILE__)

Benchmark.ips do |x|
  x.report "File.stat" do
    File.stat(existing_file)
  end
end
