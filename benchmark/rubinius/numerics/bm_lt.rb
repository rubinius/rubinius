require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i

srand(42)
fixnums = Array.new(total).fill { |a| rand(100_000) }
bignums = Array.new(total).fill { |a| 0xffff_ffff_ffff_ffff + rand(100_000) }
floats = Array.new(total).fill { |a| rand * 100_000 }

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times do |i|
      total.times do |j|
        j
      end
    end
  end

  x.report("Bignum < Bignum") do
    total.times do |i|
      total.times do |j|
        bignums[i] < bignums[j]
      end
    end
  end

  x.report("Bignum < Fixnum") do
    total.times do |i|
      total.times do |j|
        bignums[i] < fixnums[j]
      end
    end
  end

  x.report("Bignum < Float") do
    total.times do |i|
      total.times do |j|
        bignums[i] < floats[j]
      end
    end
  end

  x.report("Fixnum < Fixnum") do
    total.times do |i|
      total.times do |j|
        fixnums[i] < fixnums[j]
      end
    end
  end

  x.report("Fixnum < Bignum") do
    total.times do |i|
      total.times do |j|
        fixnums[i] < bignums[j]
      end
    end
  end

  x.report("Fixnum < Float") do
    total.times do |i|
      total.times do |j|
        fixnums[i] < floats[j]
      end
    end
  end

  x.report("Float < Float") do
    total.times do |i|
      total.times do |j|
        floats[i] < floats[j]
      end
    end
  end

  x.report("Float < Bignum") do
    total.times do |i|
      total.times do |j|
        floats[i] < bignums[j]
      end
    end
  end

  x.report("Float < Fixnum") do
    total.times do |i|
      total.times do |j|
        floats[i] < fixnums[j]
      end
    end
  end
end
