def bar(n=5)
  raise "escape" if n == 0
  bar(n-1)
end

def Bench.run
  i = 0

  while i < 50000
    bar rescue nil
    i += 1
  end
end
