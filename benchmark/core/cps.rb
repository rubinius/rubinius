module Bench
  def self.start_timer(time=5)
    @should_run = true

    Thread.new(time) do |t|
      sleep t
      @should_run = false
    end
  end

  def self.perform(time)
    t = start_timer(time)

    before = Time.now

    run

    fin = Time.now
    t.join

    diff = fin - before

    printf "%.1f : %d : %d\n" % [diff, @iterations, (@iterations / diff).round]
  end
end

file = ARGV.shift

helpers = File.join(File.dirname(file), "helpers.rb")

p helpers
if File.exists? helpers
  load helpers
end

load file

Bench.perform(1)
Bench.perform(1)
Bench.perform(1)
Bench.perform(5)
