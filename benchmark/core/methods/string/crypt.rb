def Bench.run
  i = 0
  str = random_string()
  other = "world"

  while @should_run
    str.crypt(other)
    i += 1
  end

  @iterations = i
end
