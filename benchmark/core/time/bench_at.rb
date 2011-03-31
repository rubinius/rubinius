require 'benchmark'
require 'benchmark/ips'
require 'time'
require File.expand_path('../shared_time.rb', __FILE__)

Benchmark.ips do |x|  

  fixnum = $small_time_fixnum_array.dup.first.to_i
  float = $small_time_float_array.dup.first.to_f
  

  x.report "#at with integer" do |times|
    i = 0
    while i < times
      Time.at(fixnum)
      i += 1
    end
  end

  x.report "#at with float" do |times|
    i = 0
    while i < times
      Time.at(float)
      i += 1
    end
  end

end
