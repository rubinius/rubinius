require 'benchmark'

total = (ENV['TOTAL'] || 100_000).to_i

const = 2**(total/100)

Benchmark.bmbm do |x|
  x.report("Bignum#+const") do
    total.times do |i|
      const + const
    end
  end

  x.report("Bignum#*const") do
    total.times do |i|
      const * const
    end
  end

  x.report("Bignum#+i") do
    total.times do |i|
      const + i
    end
  end

  x.report("Bignum#*i") do
    total.times do |i|
      const * i
    end
  end

  x.report("Bignum#<<2") do
    total.times do |i|
      const << 2
    end
  end

  x.report("Bignum#>>2") do
    total.times do |i|
      const >> 2
    end
  end

  x.report("Bignum#<<i") do
    total.times do |i|
      const << i
    end
  end

  x.report("Bignum#>>i") do
    total.times do |i|
      const >> i
    end
  end

  x.report("Bignum#%1000") do
    total.times do |i|
      i % 1000
    end
  end
  
  x.report("Bignum#**i") do
    total.times do |i|
      2 ** (i % 1000)
    end
  end
end
