require File.dirname(__FILE__) + '/setup'

Bench.run [1_000_000] do |n|
  n.times { h = new_hash }
end
