require 'benchmark'
require 'benchmark/ips'

existing_file = "./lib/ffi.rb"
nonexistent_file = "./lib/noexists.rb"

Benchmark.ips do |x|
  x.report "File.exists? - true" do
    File.exists?(existing_file)
  end

  x.report "File.exists? - false" do
    File.exists?(nonexistent_file)
  end
end
