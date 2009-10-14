def m
  nil
end

def Bench.run
  i=0
  while i < 15_000_000 # benchmark loop 2
    i+=1
    m; m; m; m; m; m; m; m;
  end
end
