require 'benchmark'

total = (ENV['TOTAL'] || 1_000_000).to_i

splat = [1,2,3,4,5]

def v(val)
  val
end

Benchmark.bmbm do |x|
  x.report("a = 1") do
    total.times do
      a = 1
    end
  end
  
  x.report("a,b = *splat") do
    total.times do
      a,b,c,d,e = *splat
    end
  end

  x.report("a,b = splat") do
    total.times do
      a,b,c,d,e = splat
    end
  end

  x.report("a,b = 1,2") do
    total.times do
      a,b,c,d,e = 1,2,3,4,5
    end
  end

  x.report("a=1; b=2") do
    total.times do
      a = 1
      b = 2
      c = 3
      d = 4
      e = 5
    end
  end

  x.report("a,b = v(1),v(2)") do
    total.times do
      a,b,c,d,e = v(1),v(2),v(3),v(4),v(5)
    end
  end

  x.report("a=v(1); b=v(2)") do
    total.times do
      a = v 1
      b = v 2
      c = v 3
      d = v 4
      e = v 5
    end
  end

  x.report("a,b = b,a") do
    a,b,c,d,e = 1,2,3,4,5
    total.times do
      a,b,c,d,e = e,d,c,b,a
    end
  end
end
