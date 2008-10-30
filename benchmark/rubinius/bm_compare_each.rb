require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i
size = 5_000

ByteArray = Array unless defined?(ByteArray)

array = Array.new(size) { |i| i }
byte_array = ByteArray.new(size)
string = array.join

Benchmark.bmbm do |x|
  x.report("empty loop") do
    total.times { |i| i }
  end

  x.report("ByteArray#each") do
    total.times { |i| byte_array.each { |a| a } }
  end

  x.report("Array#each") do
    total.times { |i| array.each { |a| a } }
  end

  x.report("Array#reverse_each") do
    total.times { |i| array.reverse_each { |a| a } }
  end

  x.report("String#each") do
    total.times { |i| string.each {|a| a} }
  end
  
  x.report("String#each_byte") do
    total.times { |i| string.each_byte {|a| a} }
  end
  
  x.report("String#each_char") do
    total.times { |i| string.each_char {|a| a} }
  end   
end
