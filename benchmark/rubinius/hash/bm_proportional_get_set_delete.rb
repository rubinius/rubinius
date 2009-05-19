require File.dirname(__FILE__) + '/setup'

# See the other bm_proportional_xxx benchmarks.

sizes_and_iterations.each do |max, iterations|
  Bench.run iterations do |n|
    n.times do
      h = new_hash
      (0...max).each do
        a = h[get_key(max)]
        a = h[get_key(max)]
        key = get_key(max)
        h[key] = 0
        a = h[get_key(max)]
        h[get_key(max)] = 0
        a = h[get_key(max)]
        a = h[get_key(max)]
        h.delete key
      end
    end
  end
end
