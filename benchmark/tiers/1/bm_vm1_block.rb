def m
  yield
end

def Bench.run
  10_000_000.times { m {} }
end
