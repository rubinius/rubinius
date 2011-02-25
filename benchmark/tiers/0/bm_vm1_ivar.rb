require 'benchmark'
require 'benchmark/ips'

class IVarBench
  def bench
    @a = 1

    Benchmark.ips do |x|
      x.report "ivar read" do |times|
        i = 0
        while i < times
          i += 1
          @a
        end
      end

      x.report "ivar set" do |times|
        i = 0
        while i < times
          i += 1
          @a = 1
        end
      end
    end
  end
end

IVarBench.new.bench
