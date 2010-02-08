def Bench.run
  i = 0
  while @should_run
    # string#ljust(...)
    raise "string#ljust(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
