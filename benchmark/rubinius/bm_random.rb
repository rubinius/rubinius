require 'benchmark'

total = (ENV['TOTAL'] || 10_000).to_i

Benchmark.bmbm do |x|
  x.report("Kernel#rand()") do
    total.times do
      rand
    end
  end

  x.report("Kernel#rand(10)") do
    total.times do
      rand(10)
    end
  end

  x.report("Kernel#rand(0x12345678901234567890)") do
    total.times do
      rand(0x12345678901234567890)
    end
  end

  x.report("Kernel#srand()") do
    total.times do
      srand
    end
  end

  x.report("Kernel#srand(0)") do
    total.times do
      srand(0)
    end
  end

  x.report("Kernel#srand(0x12345678901234567890)") do
    total.times do
      srand(0x12345678901234567890)
    end
  end

  x.report("Kernel#srand(10); Kernel#rand") do
    (total / 20).times do
      srand(10)
      50.times do
        rand
      end
    end
  end
end
