require 'benchmark'

total = (ENV['TOTAL'] || 1_000_000).to_i

Benchmark.bmbm do |x|  
  x.report("while") do
    i = 0
    while(i < total)
      i
      i += 1
    end
  end

  x.report("until") do
    i = 0
    until(i == total)
      i
      i += 1
    end
  end

  x.report("do while") do
    i = 0
    begin
      i
      i += 1
    end while(i < total)
  end

  x.report("do until") do
    i = 0
    begin
      i
      i += 1
    end until(i == total)
  end  

  x.report("loop") do
    i = 0
    loop do
      i += 1
      i
      break if i == total
    end
  end
  
  x.report("Integer#times") do
    total.times {|i| i}
  end

  x.report("Integer#upto") do
    0.upto(total-1) {|i| i}
  end
end
