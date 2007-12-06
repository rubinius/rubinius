require 'benchmark'

total = ENV['TOTAL'] || 1_000

total = total.to_i

class Blah
  def go(&b)
    b.call
  end

  def go2
    yield
  end
end

Benchmark.bmbm do |x|
  x.report("yield") do
    i = 0
    b = Blah.new
    while i < total
      b.go2 { true }
      i += 1
    end
  end

  x.report("prc.call") do
    i = 0
    b = Blah.new
    while i < total
      b.go { true }
      i += 1
    end
  end
end
