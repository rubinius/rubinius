require 'benchmark'
require 'benchmark/ips'

existing_file = File.join(__FILE__)
existing_dir  = File.dirname(__FILE__)

Benchmark.ips do |x|
  x.report "File.file? - true" do
    File.file?(existing_file)
  end

  x.report "File.file? - false" do
    File.file?(existing_dir)
  end
end
