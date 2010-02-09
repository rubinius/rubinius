def Bench.run
  i = 0
  str = random_string
  while @should_run
    str.inspect
    i += 1
  end

  @iterations = i
end
