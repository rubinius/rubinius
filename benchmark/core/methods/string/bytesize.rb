def Bench.run
  i = 0
  while @should_run
    # string#bytesize(...)
    raise "string#bytesize(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
