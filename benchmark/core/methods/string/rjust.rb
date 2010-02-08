def Bench.run
  i = 0
  while @should_run
    # string#rjust(...)
    raise "string#rjust(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
