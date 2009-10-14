def Bench.run
  i = 0
  while i<50_000_000 # while loop 1
    i+= 1
    @a = 1
    @b = 2
  end
end
