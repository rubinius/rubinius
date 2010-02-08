def Bench.run
  i = 0
  while @should_run
    # string#concat(...)
    raise "string#concat(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
