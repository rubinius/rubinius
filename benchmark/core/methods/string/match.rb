def Bench.run
  i = 0
  while @should_run
    # string#match(...)
    raise "string#match(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
