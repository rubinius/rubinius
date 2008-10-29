require 'benchmark'
 
total = (ENV['TOTAL'] || 5_000).to_i

array = Array.new(total) {|i| i}

Benchmark.bmbm do |x|
  x.report 'empty' do
    total.times {|i| i }
  end

  x.report 'Array#initialize' do
    total.times do |i|
      Array.new(i,0)
    end
  end

  x.report 'Array#initialize {|i| i}' do
    total.times do |i|
      Array.new(i) {|i| i}
    end
  end

  x.report 'Array#+' do
    total.times do |i|
      Array.new(i,0) + Array.new(i,0)
    end
  end
  
  x.report 'Array#include?' do
    total.times do |i|
      array.include?(i)
    end
  end

  x.report 'Array#index' do
    total.times do |i|
      array.index(i)
    end
  end

  x.report 'Array#rindex' do
    total.times do |i|
      array.rindex(i)
    end
  end
  
  x.report 'Array#reverse' do
    total.times do |i|
      array.reverse
    end
  end

  x.report 'Array#join' do
    (total).times do |i|
      Array.new(i).join
    end
  end
end
