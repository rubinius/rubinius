require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  hash = {:a => 'a', :b => 'b', :c => 'c', :d => 'd', :e => 'e', :f => 'f', :g => 'g', :h => 'h', :i => 'i', :j => 'j', 
    :k => -4, :l => -81, :m => 0, :n => 5, :o => 12, :p => -1_000_000, :q => 1, :r => 10, :s => 100, :t => 1000}

  x.report "#keys" do |times|
    i = 0
    while i < times
      hash.keys
      i += 1
    end
  end

  x.report "#values" do |times|
    i = 0
    while i < times
      hash.values
      i += 1
    end
  end

end
