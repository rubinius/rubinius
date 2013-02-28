require 'benchmark'
require 'benchmark/ips'

require 'erb'

erb_template = File.read(File.join(File.dirname(__FILE__), 'template.erb'))
engine = ERB.new(erb_template, nil, '-')

Benchmark.ips do |x|
  x.report "ERB result" do
    engine.result
  end
  
end


