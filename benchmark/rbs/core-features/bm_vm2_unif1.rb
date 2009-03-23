def m(a, b)
end

Bench.run [1_000_000, 2_000_000, 4_000_000, 8_000_000] do |n|
  n.times { m(100, 200) }
end
