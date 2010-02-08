def Bench.run
  i = 0
  str1 = random_string()
  str2 = random_string()

  while @should_run
    str1 <=> str2
    i += 1
  end

  @iterations = i
end
