require 'benchmark'

total = (ENV['TOTAL'] || 1000000).to_i

def int_alternative(n)
  Integer(n).to_s
end

if (1.0).respond_to?(:to_s_formatted, true)
  def str_alternative(s, n)
    "Hello #{s}, from #{n.send :to_s_formatted, '%4.2f'}"
  end
else
  def str_alternative(s, n)
    # This isn't representative, but it's less interesting on MRI
    "Hello #{s}, from #{'%4.2f' % n}"
  end
end

Benchmark.bmbm do |x|
  x.report("10.to_s") do
    total.times { int_alternative(10) }
  end
  x.report("'%1$d'") do
    total.times { sprintf('%1$d', 10) }
  end
  x.report("'%02d'") do
    total.times { sprintf('%02d', 10) }
  end

  x.report('"Hello #{s}, from #{f}"') do
    total.times { str_alternative('world', 123.4567) }
  end
  x.report("'Hello %s, from %4.2f'") do
    total.times { sprintf('Hello %s, from %4.2f', 'world', 123.4567) }
  end
end
