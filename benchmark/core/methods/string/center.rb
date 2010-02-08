def Bench.run
  i = 0
  str = "hello"
  while @should_run
    str.center(20)
    i += 1
  end

  @iterations = i
end
