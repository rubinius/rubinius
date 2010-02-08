def Bench.run
  i = 0
  while @should_run
    # string#bytes(...)
    raise "string#bytes(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
