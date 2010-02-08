def Bench.run
  i = 0
  while @should_run
    # string#each_line(...)
    raise "string#each_line(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
