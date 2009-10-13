def Bench.run
  100_000.times do
    h = { 47 => true }
    100.times { |i| h[i] }
  end
end
