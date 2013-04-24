require 'benchmark'
require 'benchmark/ips'

srand 100

CHARS = <<-EOS
Much near while convincing anathematically elephant bright one near
speechlessly a methodic goodness ouch less and woodchuck glanced fishily oh
this a that because therefore this cow that and but spat goodness one much
public went fancifully indirectly exorbitant the intensely inventoried scooped
that.
EOS

STRINGS = Array.new(50_000) { |x| CHARS[rand(CHARS.size), rand(CHARS.size)] }
OBJECTS = Array.new(50_000) { |x| Object.new }

def join_strings
  STRINGS.join("\n")
end

def join_objects
  OBJECTS.join("\n")
end

Benchmark.ips do |x|
  x.report "Array#join with String" do |times|
    i = 0
    while i < times
      join_strings
      i += 1
    end
  end

  x.report "Array#join with Object" do |times|
    i = 0
    while i < times
      join_strings
      i += 1
    end
  end
end
