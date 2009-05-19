require File.dirname(__FILE__) + '/setup'

sizes_and_iterations.each do |max, iterations|
  Bench.run iterations do |n|
    n.times do
      h = new_hash
      (0...max).each do
        h[get_key(max)] = 0
        a = h[get_key(max)]
      end
    end
  end
end
