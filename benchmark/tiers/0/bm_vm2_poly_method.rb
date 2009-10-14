class C1
  def m
    1
  end
end

class C2
  def m
    2
  end
end

def Bench.run
  o1 = C1.new
  o2 = C2.new

  which = false
  o = o1

  i = 0
  while i < 10_000_000 # benchmark loop 2
    if which
      o = o1
    else
      o = o2
    end

    which = !which
    o.m; o.m; o.m; o.m; o.m; o.m; o.m; o.m
    i+=1
  end
end
