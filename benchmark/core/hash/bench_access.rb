require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  small_hash = $small_hash

  x.report "hsh[i] (exists)" do |times|
    i = 0
    while i < times
      x = small_hash[:SlBkyplxcZ]
      i += 1
    end
  end

  x.report "hsh[i] (doesn't exist)" do |times|
    i = 0
    while i < times
      x = small_hash[:SlBkdfadsfs]
      i += 1
    end
  end

  x.report "hsh.fetch (exists)" do |times|
    i = 0
    while i < times
      x = small_hash.fetch(:SlBkyplxcZ)
      i += 1
    end
  end

  x.report "values_at" do |times|
    i = 0
    while i < times
      x = small_hash.values_at(700648627, 639030613, 471761289)
      i += 1
    end
  end

  default_small_hash = small_hash.dup
  default_small_hash.default = "boom"
  x.report "hsh[i] (default value)" do |times|
    i = 0
    while i < times
      x = default_small_hash[:nonexistent]
      i += 1
    end
  end

  default_proc_hash = Hash.new {|h,k| 1 }
  x.report "hsh[i] (default proc)" do |times|
    i = 0
    while i < times
      x = default_proc_hash[:nonexistent]
      i += 1
    end
  end
end
