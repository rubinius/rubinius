require 'benchmark'

total = (ENV['TOTAL'] || 10_000).to_i

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { |i| i }
  end

  x.report("File.open / #close") do
    total.times do
      File.open("/tmp/bm_io_open", "w") { |f| f }
    end
  end
end
