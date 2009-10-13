def Bench.run
  300_000.times do
    ary = []
    100.times { |i| ary << i }
  end
end
