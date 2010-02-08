def Bench.run
  i = 0
  while @should_run
    # string#slice_bang(...)
    raise "string#slice_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
