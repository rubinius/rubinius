require 'benchmark'
require 'benchmark/ips'

existing_file = "./lib/ffi.rb"
existing_dir  = "./lib/ffi"

Benchmark.ips do |x|
  x.report "File.file? - true" do
    File.file?(existing_file)
  end

  x.report "File.file? - false" do
    File.file?(existing_dir)
  end
end
