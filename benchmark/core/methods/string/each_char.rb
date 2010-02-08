def Bench.run
  i = 0
  while @should_run
    # string#each_char(...)
    raise "string#each_char(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
