require 'benchmark'

total = (ENV['TOTAL'] || 1_000_000).to_i

def bcall(n,&b)
  i = 0
  while(i < n)      
    b.call(i)
    i += 1
  end
end

def byield(n,&b)
  i = 0
  while(i < n)
    yield i
    i += 1
  end
end

Benchmark.bmbm do |x|
  x.report("yield") do
    byield(total) {|x| x}
  end

  x.report("prc.call") do
    bcall(total) {|x| x}
  end

  x.report("yield n") do
    total.times { byield(1) {|x| x} }
  end

  x.report("prc.call n") do
    total.times { bcall(1) {|x| x} }
  end
end
