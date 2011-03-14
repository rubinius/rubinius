require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

#Integer#even
  x.report "Integer#even? true" do |times|
    i = 0
    while i < times
      2.even?
      i += 1
    end
  end

  x.report "Integer#even? false" do |times|
    i = 0
    while i < times
      1.even?
      i += 1
    end
  end

  x.report "Integer#even? alternating" do |times|
    i = 0
    while i < times
      i.even?
      i += 1
    end
  end
  
#Integer#odd
  x.report "Integer#odd? true" do |times|
    i = 0
    while i < times
      1.even?
      i += 1
    end
  end

  x.report "Integer#odd? false" do |times|
    i = 0
    while i < times
      2.even?
      i += 1
    end
  end

  x.report "Integer#odd? alternating" do |times|
    i = 0
    while i < times
      i.even?
      i += 1
    end
  end

end
