require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  string = "The quick brown fox jumped over the lazy dog. Again. A rose by any other name would still smell as sweet. It was the best of times, it was the worst of times. It was the winter of our benchmark discontent. Friends, Romans, Countrymen! Lend me your ears. I lost mine in the war."

  x.report "dup strings" do |times|
    i = 0
    while i < times
      string.dup
      i += 1
    end
  end

end
