Const = 1

def Bench.run
  i = 0
  while i<100_000_000 # while loop 1
    i+= 1
    j = Const
    k = Const
  end
end
