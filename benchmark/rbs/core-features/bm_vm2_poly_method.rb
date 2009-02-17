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

Bench.run [1_000_000, 2_000_000, 4_000_000, 8_000_000] do |n|
  o1 = C1.new
  o2 = C2.new
  n.times do |i|
    o = (i % 2 == 0) ? o1 : o2
    o.m; o.m; o.m; o.m; o.m; o.m; o.m; o.m
  end
end
