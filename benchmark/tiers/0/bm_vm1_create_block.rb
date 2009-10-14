def m
  nil
end

def Bench.run
  i=0
  while i<50000000 # while loop 1
    i+=1
    m {}
  end
end
