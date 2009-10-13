def Bench.run
  i = 0
  a = :foo
  while i<6000000 # while loop 2
    case a
    when :bar
      raise
    when :baz
      raise
    when :boo
      raise
    when :foo
      i += 1
    end
  end
end
