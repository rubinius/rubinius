require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i

fixnums = Array.new(total).fill { |a| rand(100_000) }
bignums = Array.new(total).fill { |a| 0xffff_ffff_ffff_ffff + rand(100_000) }
floats = Array.new(total).fill { |a| rand * 100_000 }
strings = fixnums.map { |i| i.to_s }
values = fixnums.zip(bignums, floats, strings).flatten

Benchmark.bmbm do |x|
  x.report "loop" do
    total.times do |i|
      total.times do |j|
        obj = values[j]
      end
    end
  end

  x.report "case obj" do
    total.times do |i|
      total.times do |j|
        obj = values[j]
        case obj
        when Float, Bignum, Fixnum
          obj.to_f
        end
      end
    end
  end
  
  x.report "[].include?" do
    total.times do |i|
      total.times do |j|
        obj = values[i]
        if [Float, Bignum, Fixnum].include?(obj.class)
          obj.to_f
        end
      end
    end
  end  
end
