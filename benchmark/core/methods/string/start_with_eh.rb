def Bench.run
  i = 0
  while @should_run
    # string#start_with_eh(...)
    raise "string#start_with_eh(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
