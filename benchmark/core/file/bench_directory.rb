require 'benchmark'
require 'benchmark/ips'

existing_dir = "./lib/ffi"
nonexistent_dir = "./lib/noexists"

Benchmark.ips do |x|
  x.report "File.directory? - true" do
    File.directory?(existing_dir)
  end

  x.report "File.directory? - false" do
    File.directory?(nonexistent_dir)
  end
end
