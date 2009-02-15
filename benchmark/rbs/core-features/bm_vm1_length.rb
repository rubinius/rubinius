string = "a" * n
array = Array.new(n, 42)

Bench.run [10, 100, 1000, 10_000] do |n|
  30_000_000.times do
    string.length
    array.length
  end
end
