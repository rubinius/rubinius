require 'benchmark'
require 'benchmark/ips'

path = "./lib/ffi/generator.rb"

Benchmark.ips do |x|
  x.report "File.fnmatch plain" do
    File.fnmatch(path, path)
  end

  x.report "File.fnmatch *" do
    File.fnmatch("./lib/ffi/*.rb", path)
  end
end
