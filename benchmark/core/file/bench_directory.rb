require 'benchmark'
require 'benchmark/ips'

existing_dir = File.dirname(__FILE__)
nonexistent_dir = File.join(__FILE__, 'noexists')

Benchmark.ips do |x|
  x.report "File.directory? - true" do
    File.directory?(existing_dir)
  end

  x.report "File.directory? - false" do
    File.directory?(nonexistent_dir)
  end
end
