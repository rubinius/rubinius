require 'benchmark'
require 'benchmark/ips'

path = File.join(__FILE__)

Benchmark.ips do |x|
  x.report "File.fnmatch plain" do
    File.fnmatch(path, path)
  end

  x.report "File.fnmatch *" do
    File.fnmatch("*.rb", path)
  end
end
