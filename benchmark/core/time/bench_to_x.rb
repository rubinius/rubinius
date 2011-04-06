require 'benchmark'
require 'benchmark/ips'
require 'time'
require File.expand_path('../shared_time.rb', __FILE__)

Benchmark.ips do |x|  

  time_fixnum = $small_time_fixnum_array.dup.first
  time_float = $small_time_float_array.dup.first
  
  x.compare!
  
  x.report "fixnum Time#to_i" do |times|
    i = 0
    while i < times
      time_fixnum.to_i
      i += 1
    end
  end
  
  x.report "fixnum Time#to_f" do |times|
    i = 0
    while i < times
      time_fixnum.to_f
      i += 1
    end
  end
  
  x.report "float Time#to_i" do |times|
    i = 0
    while i < times
      time_float.to_i
      i += 1
    end
  end
  
  x.report "float Time#to_f" do |times|
    i = 0
    while i < times
      time_float.to_f
      i += 1
    end
  end

end
