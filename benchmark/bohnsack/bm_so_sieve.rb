max = Integer(ARGV.shift || 1000000)
	
sieve = [nil, nil] + (2 .. max).to_a
	
(2 .. Math.sqrt(max)).each do |i|
  next unless sieve[i]
  (i*i).step(max, i) do |j|
    sieve[j] = nil
  end
end
	
puts sieve.compact.join(", ")
