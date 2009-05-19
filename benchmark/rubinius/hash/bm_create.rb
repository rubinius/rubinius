require File.dirname(__FILE__) + '/setup'

sizes_and_iterations.each do |max, iterations|
  Bench.run iterations do |n|
    n.times { h = new_hash }
  end
end
