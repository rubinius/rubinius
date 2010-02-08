def Bench.run
  i = 0
  while @should_run
    # string#lstrip(...)
    raise "string#lstrip(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
