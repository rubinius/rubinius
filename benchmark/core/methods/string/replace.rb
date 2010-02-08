def Bench.run
  i = 0
  while @should_run
    # string#replace(...)
    raise "string#replace(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
