def Bench.run
  i = 0
  s = "hello"
  while @should_run
    s.dup << " world"
    i += 1
  end

  @iterations = i
end
