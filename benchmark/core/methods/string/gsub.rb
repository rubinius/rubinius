def Bench.run
  i = 0
  while @should_run
    # string#gsub(...)
    raise "string#gsub(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
