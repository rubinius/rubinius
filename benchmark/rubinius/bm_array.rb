require 'benchmark'

srand(42)
total = (ENV['TOTAL'] || 5_000).to_i

array = Array.new(total) {|i| i}
array_of_arrays = Array.new(total) do |i|  
  Array.new(rand(total/10)) {|j| j % 100 }
end

scratch = Array.new(total) {|i| (rand(100) == 1) ? nil : i }
portion = array_of_arrays.first(total/10)

Benchmark.bmbm do |x|
  x.report 'empty N' do
    total.times {|i| i }
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

  x.report 'Array#hash' do
    total.times do |i|
      array_of_arrays[i].hash
    end
  end

  x.report 'Array#hash(portion)' do
    portion.hash
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
      Array.new(i,0) + array_of_arrays[i]
    end
  end  

  x.report 'Array#<<' do    
    arr = Array.new
    total.times do |i|
      arr << i
    end
  end
  
  x.report 'Array#concat' do
    total.times do |i|
      Array.new(i,0).concat array_of_arrays[i]
    end
  end

  x.report 'Array#concat(rep)' do
    arr = Array.new
    total.times do |i|
      arr = Array.new if i % 100 == 0
      arr.concat array_of_arrays[i]
    end
  end
  
  x.report 'Array#[]' do
    total.times do |j|
      array[j]
    end
  end

  x.report 'Array#[i,offset]' do
    total.times do |j|
      array[total-j,j]
    end
  end

  x.report 'Array#[i..total]' do
    total.times do |j|
      array[j..total]
    end
  end

  x.report 'Array#[]=' do    
    total.times do |j|
      array[j] = j
    end
  end

  x.report 'Array#[i,offset]=' do
    arr = Array.new
    total.times do |i|
      arr = Array.new if i % 100 == 0
      arr[i,i%100] = array_of_arrays[i]
    end
  end

  x.report 'Array#==' do
    a = Array.new(total/10,0)
    b = Array.new(total/10,0)
    total.times do |i|
       a == b
    end
  end

  x.report 'Array#== dup' do
    total.times do |i|
      array_of_arrays[i] == Array.new(i%100+10) {|j| j % 100 }
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

  x.report 'Array#compact' do
    total.times do |i|
      scratch.dup.compact
    end
  end

  x.report 'Array#delete' do
    total.times do |i|
      scratch.dup.delete(nil)
    end
  end

  x.report 'Array#delete_if' do
    total.times do |i|
      scratch.dup.delete_if {|x| x == nil }
    end
  end

  x.report 'Array#unshift(0)' do
    arr = Array.new(total,0)
    total.times do |i|
      arr.unshift 0
    end
  end

  x.report 'Array#unshift(1,2,3)' do
    arr = Array.new(total,0)
    total.times do |i|
      arr.unshift(1,2,3)
    end
  end

  x.report 'Array#push(1)' do
    arr = Array.new(total,0)
    total.times do |i|
      arr.push 0
    end
  end

  x.report 'Array#pop(1)' do
    arr = Array.new(total,0)
    total.times do |i|
      arr.pop
    end
  end
  
  x.report 'Array#shift' do
    arr = Array.new(total,0) 
    total.times do |i|      
      arr.shift
    end
  end

  x.report 'Array#first' do
    total.times do |i|
      array.first(i % 500)
    end
  end

  x.report 'Array#last' do
    total.times do |i|
      array.last(i % 500)
    end
  end
  
  x.report 'Array#uniq' do    
    total.times do |i|
      array_of_arrays[i].uniq
    end
  end
  
  x.report 'Array#reverse' do
    total.times do |i|
      array_of_arrays[i].reverse
    end
  end

  x.report 'Array#join' do
    total.times do |i|
      array_of_arrays[i].join
    end
  end

  x.report 'Array#join(portion)' do
    portion.join
  end
end
