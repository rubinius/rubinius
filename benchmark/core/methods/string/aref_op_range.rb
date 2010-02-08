def Bench.run
  i = 0
  str = "blah"
  range = 0..2
  while @should_run
    str[range]
    i += 1
  end

  @iterations = i
end
