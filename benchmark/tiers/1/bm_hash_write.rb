def Bench.run
  50_000.times do
    h = {}
    100.times { |i| h[i] = 0 }
  end
end
