require 'benchmark'
 
total = (ENV['TOTAL'] || 1_000).to_i

array = Array.new(total) {|i| i}
array_of_arrays = Array.new(total) do |i|
  
  Array.new((i%100)*2) {|j| j * (i%100+1) }
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

  x.report 'Array#dup' do
    total.times do |i|
      array_of_arrays[i].dup
    end
  end

  x.report 'Array#clone' do
    total.times do |i|
      array_of_arrays[i].clone
    end
  end
  
  x.report 'Array#+' do
    total.times do |i|
      Array.new(i,0) + Array.new(i,0)
    end
  end  

  x.report 'Array#<<' do    
    total.times do
      arr = Array.new
      total.times do |i|
        arr << i
      end
    end
  end
  
  x.report 'Array#concat' do
    arr = Array.new
    total.times do |i|
      arr.concat array_of_arrays[i]
    end
  end
  
  x.report 'Array#[]' do
    total.times do
      total.times do |j|
        array[j]
      end
    end
  end

  x.report 'Array#[i,offset]' do
    total.times do
      total.times do |j|
        array[j%10,j%100]
      end
    end
  end

  x.report 'Array#[]=' do    
    total.times do
      total.times do |j|
        array[j] = j
      end
    end
  end

  x.report 'Array#==' do
    total.times do |i|
      Array.new(i,0) == Array.new(i,0)
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

  x.report 'Array#select' do
    total.times do |i|
      odd_even = i % 2
      array_of_arrays[i].select {|x| x % 2 == odd_even }
    end
  end

  x.report 'Array#unshift' do
    total.times do |i|
      arr = Array.new(i%100,0)
      (i/2).times { arr.unshift 0 }
    end
  end

  x.report 'Array#push' do
    total.times do |i|
      arr = Array.new(i%100,0)
      (i/2).times { arr.push 0 }
    end
  end

  x.report 'Array#pop' do
    total.times do |i|
      arr = Array.new(i,0)
      i.times { arr.pop }
    end
  end
  
  x.report 'Array#shift' do
    total.times do |i|
      arr = Array.new(i,0) 
      i.times { arr.shift }
    end
  end

  x.report 'Array#first' do
    total.times do |i|
      array.first(i % 100)
    end
  end

  x.report 'Array#last' do
    total.times do |i|
      array.last(i % 100)
    end
  end
  
  x.report 'Array#uniq' do    
    total.times do |i|
      Array.new(i) {|j| j % 100 }.uniq
    end
  end
  
  x.report 'Array#reverse' do
    total.times do |i|
      array.reverse
    end
  end

  x.report 'Array#join' do
    total.times do |i|
      array_of_arrays[i].join
    end
  end

  x.report 'Array#join(arrays)' do
    array_of_arrays.join
  end
end
