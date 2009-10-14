
class C
  def m a
    1
  end
end

class CC < C
  def m a
    super
  end
end

def Bench.run
  i = 0
  obj = CC.new

  while i < 15_000_000 # benchmark loop 2
    obj.m 10
    i+=1
  end
end
