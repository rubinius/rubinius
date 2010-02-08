def Bench.run
  i = 0

  while @should_run
    i += 1
  end

  @iterations = i
end
