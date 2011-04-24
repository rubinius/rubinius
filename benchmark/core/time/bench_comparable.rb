require 'benchmark'
require 'benchmark/ips'
require 'time'
require File.expand_path('../shared_time.rb', __FILE__)

Benchmark.ips do |x|  

  temp = $small_time_float_array.sort
  fixnum_time = Time.at(temp[0].to_i)
  fixnum_time2 = Time.at(temp[0].to_i)
  
  float_time = temp[0]
  float_time2 = temp[0]
  
  x.compare!
  
  x.report "time < time, (fixnum <=> fixnum)" do |times|
    i = 0
    while i < times
      fixnum_time < fixnum_time
      i += 1
    end
  end
  
  x.report "time <= time, (fixnum <=> fixnum)" do |times|
    i = 0
    while i < times
      fixnum_time <= fixnum_time2
      i += 1
    end
  end
  
  x.report "time == time, (fixnum <=> fixnum)" do |times|
    i = 0
    while i < times
      fixnum_time == fixnum_time2
      i += 1
    end
  end
  
  x.report "time >= time, (fixnum <=> fixnum)" do |times|
    i = 0
    while i < times
      fixnum_time >= fixnum_time2
      i += 1
    end
  end
  
  x.report "time > time, (fixnum <=> fixnum)" do |times|
    i = 0
    while i < times
      fixnum_time > fixnum_time
      i += 1
    end
  end
  
  x.report "time < time, (float <=> float)" do |times|
    i = 0
    while i < times
      float_time < float_time
      i += 1
    end
  end
  
  x.report "time <= time, (float <=> float)" do |times|
    i = 0
    while i < times
      float_time <= float_time2
      i += 1
    end
  end
  
  x.report "time == time, (float <=> float)" do |times|
    i = 0
    while i < times
      float_time == float_time2
      i += 1
    end
  end
  
  x.report "time >= time, (float <=> float)" do |times|
    i = 0
    while i < times
      float_time >= float_time2
      i += 1
    end
  end
  
  x.report "time > time, (float <=> float)" do |times|
    i = 0
    while i < times
      float_time > float_time
      i += 1
    end
  end

  x.report "time < time, (fixnum <=> float)" do |times|
    i = 0
    while i < times
      fixnum_time < float_time
      i += 1
    end
  end
  
  x.report "time <= time, (fixnum <=> float)" do |times|
    i = 0
    while i < times
      fixnum_time <= float_time
      i += 1
    end
  end
  
  x.report "time == time, (fixnum <=> float)" do |times|
    i = 0
    while i < times
      fixnum_time == float_time
      i += 1
    end
  end
  
  x.report "time >= time, (fixnum <=> float)" do |times|
    i = 0
    while i < times
      fixnum_time >= float_time
      i += 1
    end
  end
  
  x.report "time > time, (fixnum <=> float)" do |times|
    i = 0
    while i < times
      fixnum_time > float_time
      i += 1
    end
  end

end
