iter = 0

Bench.run [500_000] do |n|
  iter += 1
  puts iter

  n.times do |i|
    Object.const_set("CONST#{iter}#{i}", 1)
    const = Object.const_get("CONST#{iter}#{i}")
  end
end
