inputs  = [10, 100, 1000, 10_000]
strings = inputs.map { |n| "a" * n }
arrays  = inputs.map { |n| Array.new n, 42 }
idx     = -1

Bench.run inputs do |n|
  string = strings[idx+=1]
  array  = arrays[idx]

  30_000_000.times do
    string.length
    array.length
  end
end
