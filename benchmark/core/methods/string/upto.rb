def Bench.run
  i = 0
  while @should_run
    # string#upto(...)
    raise "string#upto(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
