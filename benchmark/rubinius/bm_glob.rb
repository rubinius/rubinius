require 'benchmark'

total = (ENV['TOTAL'] || 10).to_i

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { Dir[''] }
  end

  x.report("Dir[]") do
    total.times { Dir["spec/**/*_spec.rb"] }
  end
end
