def Bench.run
  i = 0
  while @should_run
    # string#squeeze_bang(...)
    raise "string#squeeze_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
