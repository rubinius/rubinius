Bench.run [1000, 10_000, 100_000] do |n|
  n.times { Thread.new{}.join }
end
