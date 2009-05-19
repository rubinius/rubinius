require File.dirname(__FILE__) + '/setup'

sizes_and_iterations.each do |max, iterations|
  Bench.run iterations do |n|
    n.times do
      h = new_hash
      (0...max).each do
        h[get_key] = 0
      end
    end
  end
end
