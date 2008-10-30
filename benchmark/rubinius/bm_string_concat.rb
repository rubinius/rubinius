require 'benchmark'
 
total = (ENV['TOTAL'] || 5_000).to_i

array = Array.new(total) {|i| i}

Benchmark.bmbm do |x|
  x.report 'empty' do
    total.times {|i| i }
  end

  x.report 'String#<<(fixed)' do
    str1 = "helluva"
    str2 = "fool"
    total.times do |i|
      str1 << str2
    end
  end

  x.report 'String#<<(increasing)' do
    total.times do |i|
      str1 = "foo" * i % 100
      str2 = "a" * i % 100
      25.times do
        str1 << str2
      end
    end
  end

  x.report 'String#+(fixed)' do
    str1 = "helluva"
    str2 = "fool"
    total.times do |i|
      str1 = str1 + str2
    end
  end

  x.report 'String#+(increasing)' do
    total.times do |i|
      str1 = "foo" * i % 100
      str2 = "a" * i % 100
      25.times do
        str1 = str1 + str2
      end
    end
  end
  
  x.report 'String#+=(fixed)' do
    str1 = "helluva"
    str2 = "fool"
    total.times do |i|
      str1 += str2
    end
  end

  x.report 'String#+=(increasing)' do
    total.times do |i|
      str1 = "foo" * i % 100
      str2 = "a" * i % 100
      25.times do
        str1 += str2
      end
    end
  end
end
