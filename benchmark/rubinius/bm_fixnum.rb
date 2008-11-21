require 'benchmark'

total = (ENV['TOTAL'] || 100_000).to_i

const = 2 ** 12

Benchmark.bmbm do |x|
  x.report("Fixnum#+const") do
    total.times do |i|
      const + const
    end
  end

  x.report("Fixnum#*const") do
    total.times do |i|
      const * const
    end
  end

  x.report("Fixnum#+i") do
    total.times do |i|
      const + i
    end
  end

  x.report("Fixnum#*i") do
    total.times do |i|
      const * i
    end
  end

  x.report("Fixnum#<<2") do
    total.times do |i|
      i << 2
    end
  end

  x.report("Fixnum#>>2") do
    total.times do |i|
      i >> 2
    end
  end

  x.report("Fixnum#%29") do
    total.times do |i|
      i % 29
    end
  end

  x.report("Fixnum#**i") do
    total.times do |i|
      2 ** (i % 29)
    end
  end
end
