require 'benchmark'
 
total = (ENV['TOTAL'] || 1_000).to_i

unless defined?(Tuple)
  Tuple = Array
  class Tuple
    def swap(i,j)
      temp = at(i)
      put(i, at(j))
      put(j, temp)
    end

    alias :at :[]
    alias :put :[]=
  end
end

tuple = Tuple.new(total)
total.times do |i|
  tuple.put(i,i)
end

Benchmark.bmbm do |x|
  x.report 'empty N' do
    total.times {|i| i }
  end

  x.report 'empty N^2' do
    total.times {|i| total.times {|j| j } }
  end

  x.report 'empty N(N+1)/2' do
    total.times {|i| i.times {|j| j } }
  end
  
  x.report 'Tuple#initialize' do
    total.times do |i|
      Tuple.new(i)
    end
  end

  x.report 'Tuple#to_a' do
    total.times do |i|
      Tuple.new(i).to_a
    end
  end

  x.report 'Tuple#at' do
    total.times do 
      total.times do |i|
        tuple.at(i)
      end
    end
  end

  x.report 'Tuple#put' do
    total.times do 
      total.times do |i|
        tuple.put(i,i)
      end
    end
  end

  x.report 'Tuple#===' do
    total.times do
      tuple === tuple
    end
  end
  
  x.report 'Tuple#each' do
    total.times do
      tuple.each {|j| j}
    end
  end

  x.report 'Tuple#+' do
    total.times do |i|
      Tuple.new(i) + Tuple.new(i)
    end
  end

  x.report 'Tuple#swap' do
    total.times do 
      total.times do |i|
        tuple.swap(i,total-i-1)
      end
    end
  end

  x.report 'Tuple#join' do
    (total/10).times do
      tuple.join(",")
    end
  end

  x.report 'Tuple#shift' do
    total.times do |i|
      tup = Tuple.new(i)
      (i/2).times do
        tup.shift
      end
    end
  end
end
