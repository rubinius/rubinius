def Bench.run
  i = 0
  while @should_run
    # string#lstrip_bang(...)
    raise "string#lstrip_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
