require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i

srand(42)
fixnums = Array.new(total).fill { |a| rand(100_000) }
bignums = Array.new(total).fill { |a| 0xffff_ffff_ffff_ffff + rand(100_000) }
floats = Array.new(total).fill { |a| rand * 100_000 }

Benchmark.bmbm do |x|
  x.report "loop" do
    total.times do |i|
      total.times do |j|
        fixnums[i]
        fixnums[j]
      end
    end
  end
  
  x.report "Bignum#coerce Bignum" do
    total.times do |i|
      total.times do |j|
        bignums[i].coerce bignums[j]
      end
    end
  end
  
  x.report "Bignum#coerce Fixnum" do
    total.times do |i|
      total.times do |j|
        bignums[i].coerce fixnums[j]
      end
    end
  end
  
  if Object.const_defined? :RUBY_ENGINE and RUBY_ENGINE == 'rbx'
    x.report "Bignum#coerce Float" do
      total.times do |i|
        total.times do |j|
          bignums[i].coerce floats[j]
        end
      end
    end
  end
  
  x.report "Fixnum#coerce Fixnum" do
    total.times do |i|
      total.times do |j|
        fixnums[i].coerce fixnums[j]
      end
    end
  end
  
  x.report "Fixnum#coerce Bignum" do
    total.times do |i|
      total.times do |j|
        fixnums[i].coerce bignums[j]
      end
    end
  end
  
  x.report "Fixnum#coerce Float" do
    total.times do |i|
      total.times do |j|
        fixnums[i].coerce floats[j]
      end
    end
  end
  
  x.report "Float#coerce Float" do
    total.times do |i|
      total.times do |j|
        floats[i].coerce floats[j]
      end
    end
  end
  
  x.report "Float#coerce Bignum" do
    total.times do |i|
      total.times do |j|
        floats[i].coerce bignums[j]
      end
    end
  end
  
  x.report "Float#coerce Fixum" do
    total.times do |i|
      total.times do |j|
        floats[i].coerce fixnums[j]
      end
    end
  end
end
