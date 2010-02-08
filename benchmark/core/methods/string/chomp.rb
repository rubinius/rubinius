def Bench.run
  i = 0
  str = "hello\n\n"
  while @should_run
    str.chomp
    i += 1
  end

  @iterations = i
end
