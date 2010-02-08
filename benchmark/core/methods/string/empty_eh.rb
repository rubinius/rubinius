def Bench.run
  i = 0
  while @should_run
    # string#empty_eh(...)
    raise "string#empty_eh(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
