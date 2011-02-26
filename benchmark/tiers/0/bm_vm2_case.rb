require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "case" do |times|
    i = 0
    a = :foo
    while i < times
      case a
      when :bar
        raise
      when :baz
        raise
      when :boo
        raise
      when :foo
        i += 1
      end
    end
  end
end
