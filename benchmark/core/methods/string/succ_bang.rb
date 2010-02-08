def Bench.run
  i = 0
  while @should_run
    # string#succ_bang(...)
    raise "string#succ_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
