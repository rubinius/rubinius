def Bench.run
  i = 0
  while @should_run
    # string#slice(...)
    raise "string#slice(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
