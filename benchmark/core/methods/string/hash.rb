def Bench.run
  i = 0
  while @should_run
    # string#hash(...)
    raise "string#hash(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
