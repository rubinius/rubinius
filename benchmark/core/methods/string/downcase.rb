def Bench.run
  i = 0
  str = random_string()
  while @should_run
    str.downcase
    i += 1
  end

  @iterations = i
end
