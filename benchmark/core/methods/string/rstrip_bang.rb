def Bench.run
  i = 0
  while @should_run
    # string#rstrip_bang(...)
    raise "string#rstrip_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
