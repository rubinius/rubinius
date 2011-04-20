require 'benchmark'
require 'benchmark/ips'

class Foo
  def initialize(s, i, f)
    @s = s
    @i = i
    @f = f
  end
end

Benchmark.ips do |x|

  x.report '#dump with a short string' do |times|
    i = 0
    while i < times
      Marshal.dump('hello, world')
      i += 1
    end
  end

  x.report '#dump with a long string' do |times|
    i = 0
    while i < times
      Marshal.dump('hello, world' * 10_000)
      i += 1
    end
  end

  x.report '#dump with a small integer' do |times|
    i = 0
    while i < times
      Marshal.dump(137)
      i += 1
    end
  end

  x.report '#dump with a large integer' do |times|
    i = 0
    while i < times
      Marshal.dump(602_300_000_000_000_000_000_000)
      i += 1
    end
  end

  x.report '#dump with a float' do |times|
    i = 0
    while i < times
      Marshal.dump(3.141592653)
      i += 1
    end
  end

  x.report '#dump with an object' do |times|
    i = 0
    while i < times
      Marshal.dump(Foo.new('hello, world', 137, 3.141592653))
      i += 1
    end
  end
end