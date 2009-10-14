def m
  return 1
end

def Bench.run
  i=0
  while i < 100_000_000 # while loop 1
    i+=1
    m
  end
end

