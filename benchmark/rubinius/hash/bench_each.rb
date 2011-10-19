require 'benchmark'
require 'benchmark/ips'

require File.expand_path('../setup', __FILE__)

Benchmark.ips do |x|
  [10, 10000].each do |n|
    h = new_sym_hash n
    key = value = nil

    x.report "each sym #{n}" do |t|
      i = 0
      while i < t
        h.each { |k,v| key = k; value = v }
        i += 1
      end
    end

    x.report "each_pair sym #{n}" do |t|
      i = 0
      while i < t
        h.each_pair { |k,v| key = k; value = v }
        i += 1
      end
    end
  end
end
