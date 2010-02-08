def Bench.run
  i = 0
  while @should_run
    # string#squeeze(...)
    raise "string#squeeze(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
