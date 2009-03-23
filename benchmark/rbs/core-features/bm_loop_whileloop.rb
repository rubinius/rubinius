def while_loop(n)
  i = 0
  while i < n
    i += 1
  end
end

Bench.run [100_000, 1_000_000, 10_000_000] do |n|
  while_loop n
end
