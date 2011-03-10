require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  
  x.report "reject all" do |times|
    i = 0
    while i < times
      small_hash.reject { |k,v| true }
      i += 1
    end
  end

  x.report "reject none" do |times|
    i = 0
    while i < times
      small_hash.reject { |k,v| false }
      i += 1
    end
  end

  x.report "reject! all" do |times|
    i = 0
    while i < times
      hash = small_hash.dup # must dup since #reject! modifies receiver
      hash.reject! { |k,v| true }
      i += 1
    end
  end

  x.report "reject! none" do |times|
    i = 0
    while i < times
      small_hash.reject! { |k,v| false }
      i += 1
    end
  end

  x.report "delete_if all" do |times|
    i = 0
    while i < times
      hash = small_hash.dup # must dup since #reject! modifies receiver
      hash.delete_if { |k,v| true }
      i += 1
    end
  end

  x.report "delete_if none" do |times|
    i = 0
    while i < times
      small_hash.delete_if { |k,v| false }
      i += 1
    end
  end

  x.report "delete an item that's there" do |times|
    i = 0
    while i < times
      hash = small_hash.dup
      hash.delete(700648627)
      i += 1
    end
  end
  x.report "delete an item that's not there" do |times|
    i = 0
    while i < times
      small_hash.delete(:notthere)
      i += 1
    end
  end

  empty = {}
  x.report "clear empty" do |times|
    i = 0
    while i < times
      empty.clear
      i += 1
    end
  end

  x.report "clear full" do |times|
    i = 0
    while i < times
      hash = small_hash.dup
      hash.clear
      i += 1
    end
  end
end
