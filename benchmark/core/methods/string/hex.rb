def Bench.run
  i = 0
  while @should_run
    # string#hex(...)
    raise "string#hex(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
