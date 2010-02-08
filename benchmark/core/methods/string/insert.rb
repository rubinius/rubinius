def Bench.run
  i = 0
  while @should_run
    # string#insert(...)
    raise "string#insert(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
