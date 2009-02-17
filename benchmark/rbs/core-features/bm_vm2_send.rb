class C
  def m
  end
end

Bench.run [1_000_000, 2_000_000, 4_000_000, 8_000_000] do |n|
  o = C.new
  n.times { o.__send__ :m }
end
