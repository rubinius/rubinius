require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i
range = Range.new(0,total)

stotal = total / 10
srange = Range.new("A","A"*(Math::log(stotal)))

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times {}
  end

  x.report("Range#initialize(Fixnum)") do
    total.times do |i|
      Range.new(0,i)
    end
  end

  x.report("Range#hash(Fixnum)") do
    total.times do |i|
      range.hash
    end
  end

  x.report("Range#each(Fixnum)") do
    total.times do |i|
      range.each {|x| x}
    end
  end

  x.report("Range#step(Fixnum)") do
    total.times do |i|
      range.step {|x| x}
    end
  end

  x.report("Range#inspect(Fixnum)") do
    total.times do |i|
      range.inspect
    end
  end

  x.report("Range#to_s(Fixnum)") do
    total.times do |i|
      range.to_s
    end
  end

  x.report("Range#to_a(Fixnum)") do
    total.times do |i|
      range.to_a
    end
  end
  
  x.report("Range#initialize(String)") do
    total.times do |i|
      Range.new("A","AAA")
    end
  end

  x.report("Range#hash(String)") do
    total.times do |i|
      range.hash
    end
  end

  x.report("Range#each(String)") do
    total.times do |i|
      range.each {|x| x}
    end
  end

  x.report("Range#step(String)") do
    total.times do |i|
      range.step {|x| x}
    end
  end

  x.report("Range#inspect(String)") do
    total.times do |i|
      range.inspect
    end
  end

  x.report("Range#to_s(String)") do
    total.times do |i|
      range.to_s
    end
  end

  x.report("Range#to_a(String)") do
    total.times do |i|
      range.to_a
    end
  end  
end
