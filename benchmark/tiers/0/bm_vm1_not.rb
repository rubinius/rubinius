def Bench.run
  i = 0
  obj = Object.new

  while i<100_000_000 # while loop 1
    i+= 1
    !obj
  end
end
