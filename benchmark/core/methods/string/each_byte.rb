def Bench.run
  i = 0
  while @should_run
    # string#each_byte(...)
    raise "string#each_byte(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
