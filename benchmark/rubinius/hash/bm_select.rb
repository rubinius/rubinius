require File.dirname(__FILE__) + '/setup'

# We use a single fixed key since the iteration
# will need to visit every entry anyway and this
# removes possible variations due to the size of
# the returned array.
sizes_and_iterations.each do |max, iterations|
  h = new_hash
  (0...max).each do
    h[get_key(max)] = 0
  end
  selected = nil
  key = get_key(max)

  Bench.run iterations do |n|
    (n*8).times do
      selected = h.select { |k, v| k == key }
    end
  end

  selected
end
