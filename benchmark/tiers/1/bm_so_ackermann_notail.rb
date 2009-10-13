def ack(m, n)
  if n == 0
    ack(m - 1, 1)
  elsif m != 0
    ack(m - 1, ack(m, n - 1))
  else
    n + 1
  end
end

def Bench.run
  50.times { ack 3, 7 }
end
