require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i
fixnums = Array.new(total) {|x| x}
truths = Array.new(total) {|x| true}
falses = Array.new(total) {|x| false}

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { }
  end

  x.report("Enumerable#grep &block") do
    total.times do |i|
      fixnums.grep(i..total) {|x| x}
    end
  end

  x.report("Enumerable#grep") do
    total.times do |i|
      fixnums.grep(i..total)
    end
  end

  x.report("Enumerable#all? &block") do
    total.times do |i|
      truths.all? {|x| x }
    end
  end

  x.report("Enumerable#all?") do
    total.times do |i|
      truths.all? 
    end
  end

  
  x.report("Enumerable#any? &block") do
    total.times do |i|
      falses.any? {|x| x }
    end
  end

  x.report("Enumerable#any?") do
    total.times do |i|
      falses.any? 
    end
  end
  
  x.report("Enumerable#one? &block") do
    total.times do |i|
      falses[i] = true
      falses.one? {|x| x}
      falses[i] = false
    end
  end

  x.report("Enumerable#one?") do
    total.times do |i|
      falses[i] = true
      falses.one?
      falses[i] = false
    end
  end
  
  x.report("Enumerable#none? &block") do
    total.times do |i|
      falses.none? {|x| x }
    end
  end
  
  x.report("Enumerable#none?") do
    total.times do |i|
      falses.none? 
    end
  end

  x.report("Enumerable#min &block") do
    total.times do |i|
      fixnums.min {|a,b| a <=> b }
    end
  end
  
  x.report("Enumerable#min") do
    total.times do |i|
      fixnums.min
    end
  end

  x.report("Enumerable#max &block") do
    total.times do |i|
      fixnums.max {|a,b| a <=> b }
    end
  end
  
  x.report("Enumerable#max") do
    total.times do |i|
      fixnums.max
    end
  end  
end
