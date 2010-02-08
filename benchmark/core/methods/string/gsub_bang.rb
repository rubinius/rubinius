def Bench.run
  i = 0
  while @should_run
    # string#gsub_bang(...)
    raise "string#gsub_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
