# Benchmarks non local return

def bar(n=5, &block)
  block.call if n == 0
  bar(n-1, &block)
end

def doit
  bar { return }
end

def Bench.run
  i = 0

  while i < 100000
    doit
    i += 1
  end
end
