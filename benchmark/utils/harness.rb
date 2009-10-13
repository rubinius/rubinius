unless ENV['NO_TIMEOUT']
  Thread.new do
    sleep 60
    STDOUT.puts "TIMEOUT"
    STDOUT.flush
    exit! 1
  end
end

module Bench
  def self.iterate
    start = Time.now
    run
    fin = Time.now
    return ((fin.to_f - start.to_f) * 1_000).to_i
  end
end

file = ARGV.shift

load file

# warmup
Bench.run

total = (ENV['TIMES'] || 5).to_i

times = []
total.times { times << Bench.iterate }

STDOUT.puts "TIME #{times.join(' ')}"
