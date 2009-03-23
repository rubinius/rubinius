def m
  yield
end

Bench.run [100_000, 1_000_000, 10_000_000] do |n|
  n.times { m {} }
end
