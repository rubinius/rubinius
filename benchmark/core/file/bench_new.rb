require 'benchmark'
require 'benchmark/ips'

existing_file = File.join(__FILE__)

Benchmark.ips do |x|
  x.report "File.new" do
    File.new(existing_file)
  end
end
