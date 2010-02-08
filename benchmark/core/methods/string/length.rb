def Bench.run
  i = 0
  while @should_run
    # string#length(...)
    raise "string#length(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
