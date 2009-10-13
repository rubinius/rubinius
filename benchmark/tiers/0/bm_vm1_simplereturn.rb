def m
  return 1
end

def Bench.run
  i=0
  while i<30000000 # while loop 1
    i+=1
    m
  end
end

