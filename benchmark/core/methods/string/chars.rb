def Bench.run
  i = 0
  while @should_run
    # string#chars(...)
    raise "string#chars(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
