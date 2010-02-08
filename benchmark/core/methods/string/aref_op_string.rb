def Bench.run
  i = 0
  str = "blah"
  while @should_run
    str["la"]
    i += 1
  end

  @iterations = i
end
