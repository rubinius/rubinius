require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  part = "aaaa"

  x.report "string interpolation" do |times|
    i = 0
    while i < times
      "#{part} #{part} #{part}"
      i += 1
    end
  end

  x.report "string concat with #+" do |times|
    i = 0
    while i < times
      part + part + part
      i += 1
    end
  end

  x.report "string concat with #<< and dup" do |times|
    i = 0
    while i < times
      part2 = part.dup
      part2 << part2 << part2
      i += 1
    end
  end

  x.report "string concat with #<< and clone" do |times|
    i = 0
    while i < times
      part2 = part.clone
      part2 << part2 << part2
      i += 1
    end
  end

x.report "string concat with #<< and new string" do |times|
    i = 0
    while i < times
      part = "aaaa"
      part << part << part
      i += 1
    end
  end

x.report "string concat with #concat" do |times|
    i = 0
    while i < times
      part2 = part.dup
      part2.concat(part2).concat(part2)
      i += 1
    end
  end


x.report "string multiplication" do |times|
    i = 0
    while i < times
      part * 3
      i += 1
    end
  end



end
