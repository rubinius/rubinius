require 'benchmark'

n = (ARGV.shift || 1_000).to_i
i = (ARGV.shift || 10).to_i

c = -(n / 2)
str = 'Hello there how goes?'

Benchmark.bmbm {|bench|
  bench.report("Numeric Array") { i.times { Array.new(n) { c += 1 } } }

  bench.report("String Array") {
    i.times {
      strings = Array.new(n).map! {
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}" \
        "#{(65 + rand(58)).chr}"[0..(rand(15 + 6))]
      }
    }
  }
}
