def Bench.run
  i = 0
  while @should_run
    # string#strip(...)
    raise "string#strip(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
