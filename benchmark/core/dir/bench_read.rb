require 'benchmark'
require 'benchmark/ips'

path = "lib"

Benchmark.ips do |x|
  x.report "Dir#read" do
    Dir.open(path) do |dir|
      while dir.read; end
    end
  end
end
