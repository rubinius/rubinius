require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i

srand(42)
bignums = Array.new(total*3).fill { |a| 0xffff_ffff_ffff_ffff + rand(100_000) }
offsets = Array.new(total*3).fill { |a| rand(128) }
exponents = Array.new(total*2).fill { |a| rand(128) }
exponents += Array.new(total).fill { |a| rand * 128 }
numerics = Array.new(total).fill { |a| rand(100_000) }
numerics += Array.new(total).fill { |a| 0xffff_ffff_ffff_ffff + rand(100_000) }
numerics += Array.new(total).fill { |a| rand * 100_000 }
pnumerics = numerics.map {|x| x == 0 ? x + 1e-20 : x }
total *= 3

Benchmark.bmbm do |x|
  x.report "loop" do
    total.times do |i|
      total.times do |j|
        j
      end
    end
  end

  x.report "Bignum -@" do
    total.times do |i|
      total.times do |j|
        -bignums[i]
      end
    end
  end
  
  x.report "Bignum +" do
    total.times do |i|
      total.times do |j|
        bignums[i] + numerics[j]
      end
    end
  end
  
  x.report "Bignum -" do
    total.times do |i|
      total.times do |j|
        bignums[i] - numerics[j]
      end
    end
  end
  
  x.report "Bignum *" do
    total.times do |i|
      total.times do |j|
        bignums[i] * numerics[j]
      end
    end
  end
  
  x.report "Bignum /" do
    total.times do |i|
      total.times do |j|
        bignums[i] / pnumerics[j]
      end
    end
  end
  
  x.report "Bignum %" do
    total.times do |i|
      total.times do |j|
        bignums[i] % pnumerics[j]
      end
    end
  end
  
  x.report "Bignum <<" do
    total.times do |i|
      total.times do |j|
        bignums[i] << offsets[j]
      end
    end
  end
  
  x.report "Bignum >>" do
    total.times do |i|
      total.times do |j|
        bignums[i] >> offsets[j]
      end
    end
  end
  
  x.report "Bignum <" do
    total.times do |i|
      total.times do |j|
        bignums[i] < numerics[j]
      end
    end
  end
  
  x.report "Bignum <=" do
    total.times do |i|
      total.times do |j|
        bignums[i] <= numerics[j]
      end
    end
  end
  
  x.report "Bignum >" do
    total.times do |i|
      total.times do |j|
        bignums[i] > numerics[j]
      end
    end
  end
  
  x.report "Bignum >=" do
    total.times do |i|
      total.times do |j|
        bignums[i] >= numerics[j]
      end
    end
  end
  
  x.report "Bignum ==" do
    total.times do |i|
      total.times do |j|
        bignums[i] == numerics[j]
      end
    end
  end

  x.report "Bignum <=>" do
    total.times do |i|
      total.times do |j|
        bignums[i] <=> numerics[j]
      end
    end
  end

  x.report "Bignum **" do
    total.times do |i|
      total.times do |j|
        bignums[i] ** exponents[j]
      end
    end
  end
end
