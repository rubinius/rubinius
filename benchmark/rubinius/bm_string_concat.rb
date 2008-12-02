require 'benchmark'
 
total = (ENV['TOTAL'] || 1_500).to_i
inside = total / 10

unless "".respond_to? :append
  class String
    alias :append :<<
  end
end

Benchmark.bmbm do |x|
  x.report 'empty' do
    total.times {|i| inside.times {|j| j } }
  end

  x.report 'String#append(fixed)' do
    total.times do |i|
      str1 = "helluva"
      str2 = "fool"
      inside.times do 
        str1.append str2
      end
    end
  end

  x.report 'String#append(increasing)' do
    total.times do |i|
      str1 = "foo" * (i % 100)
      str2 = "a" * (i % 100)
      inside.times do
        str1.append str2
      end
    end
  end
  
  x.report 'String#<<(fixed)' do
    total.times do |i|
      str1 = "helluva"
      str2 = "fool"
      inside.times do 
        str1 << str2
      end
    end
  end

  x.report 'String#<<(increasing)' do
    total.times do |i|
      str1 = "foo" * (i % 100)
      str2 = "a" * (i % 100)
      inside.times do
        str1 << str2
      end
    end
  end

  x.report 'String#concat(fixed)' do
    total.times do |i|
      str1 = "helluva"
      str2 = "fool"
      inside.times do
        str1.concat str2
      end
    end
  end

  x.report 'String#concat(increasing)' do
    total.times do |i|
      str1 = "foo" * (i % 100)
      str2 = "a" * (i % 100)
      inside.times do
        str1.concat str2
      end
    end
  end

  x.report 'String#+(fixed)' do
    total.times do |i|
      str1 = "helluva"
      str2 = "fool"
      inside.times do
        str1 = str1 + str2
      end
    end
  end

  x.report 'String#+(increasing)' do
    total.times do |i|
      str1 = "foo" * (i % 100)
      str2 = "a" * (i % 100)
      inside.times do
        str1 = str1 + str2
      end
    end
  end
  
  x.report 'String#+=(fixed)' do
    total.times do |i|
      str1 = "helluva"
      str2 = "fool"
      inside.times do 
        str1 += str2
      end
    end
  end

  x.report 'String#+=(increasing)' do
    total.times do |i|
      str1 = "foo" * (i % 100)
      str2 = "a" * (i % 100)
      inside.times do
        str1 += str2
      end
    end
  end
end
