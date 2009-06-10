require File.dirname(__FILE__) + '/setup'

sizes_and_iterations.each do |max, iterations|
  h = new_hash
  (0...max).each do
    h[get_key(max)] = 0
  end
  entry = nil

  Bench.run iterations do |n|
    (n*8).times do
      h.each { |e| entry = e }
    end
  end

  entry
end
