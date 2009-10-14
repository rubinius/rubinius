def Bench.run
  @a = 1

  i = 0
  while i<50_000_000 # while loop 1
    i+= 1
    j = @a
    k = @a
  end
end
