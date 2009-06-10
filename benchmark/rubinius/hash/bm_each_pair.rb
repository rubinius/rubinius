require File.dirname(__FILE__) + '/setup'

sizes_and_iterations.each do |max, iterations|
  h = new_hash
  (0...max).each do
    h[get_key(max)] = 0
  end
  key, value = nil, nil

  Bench.run iterations do |n|
    (n*8).times do
      h.each_pair { |k, v| key = k; value = v }
    end
  end

  [key, value]
end
