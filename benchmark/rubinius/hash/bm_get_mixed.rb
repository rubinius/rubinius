require File.dirname(__FILE__) + '/setup'

sizes_and_iterations.each do |max, iterations|
  h = new_hash
  (0...max).each do
    h[get_key(max)] = 0
  end

  Bench.run iterations do |n|
    n.times do
      (0...max).each do
        a = h[get_key(max * 2)]
      end
    end
  end
end
