def Bench.run
  i = 0
  str = "blah"
  while @should_run
    str[0,2]
    i += 1
  end

  @iterations = i
end
