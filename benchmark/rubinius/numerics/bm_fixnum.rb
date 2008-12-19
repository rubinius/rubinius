require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i

srand(42)
fixnums = Array.new(total*3) { rand(100_000) }
ordered_fixnums = Array.new(total*3) {|x| x}
offsets = Array.new(total*3) { rand(32) }
exponents = Array.new(total*2) { rand(64) }
exponents += Array.new(total) { rand * 64 }
numerics = Array.new(total) { rand(100_000) }
numerics += Array.new(total) { 0xffff_ffff_ffff_ffff + rand(100_000) }
numerics += Array.new(total) { rand * 100_000 }
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

  x.report "Fixnum -@" do
    total.times do |i|
      total.times do |j|
        -fixnums[i]
      end
    end
  end
  
  x.report "Fixnum +" do
    total.times do |i|
      total.times do |j|
        fixnums[i] + numerics[j]
      end
    end
  end
  
  x.report "Fixnum -" do
    total.times do |i|
      total.times do |j|
        fixnums[i] - numerics[j]
      end
    end
  end
  
  x.report "Fixnum *" do
    total.times do |i|
      total.times do |j|
        fixnums[i] * numerics[j]
      end
    end
  end
  
  x.report "Fixnum /" do
    total.times do |i|
      total.times do |j|
        fixnums[i] / pnumerics[j]
      end
    end
  end
  
  x.report "Fixnum %" do
    total.times do |i|
      total.times do |j|
        fixnums[i] % pnumerics[j]
      end
    end
  end
  
  x.report "Fixnum <<" do
    total.times do |i|
      total.times do |j|
        fixnums[i] << offsets[j]
      end
    end
  end
  
  x.report "Fixnum >>" do
    total.times do |i|
      total.times do |j|
        fixnums[i] >> offsets[j]
      end
    end
  end
  
  x.report "Fixnum <" do
    total.times do |i|
      total.times do |j|
        fixnums[i] < numerics[j]
      end
    end
  end
  
  x.report "Fixnum <=" do
    total.times do |i|
      total.times do |j|
        fixnums[i] <= numerics[j]
      end
    end
  end
  
  x.report "Fixnum >" do
    total.times do |i|
      total.times do |j|
        fixnums[i] > numerics[j]
      end
    end
  end
  
  x.report "Fixnum >=" do
    total.times do |i|
      total.times do |j|
        fixnums[i] >= numerics[j]
      end
    end
  end
  
  x.report "Fixnum ==" do
    total.times do |i|
      total.times do |j|
        fixnums[i] == numerics[j]
      end
    end
  end

  x.report "Fixnum <=>" do
    total.times do |i|
      total.times do |j|
        fixnums[i] <=> numerics[j]
      end
    end
  end

  x.report "Fixnum **" do
    total.times do |i|
      total.times do |j|
        ordered_fixnums[i] ** exponents[j]
      end
    end
  end
end
