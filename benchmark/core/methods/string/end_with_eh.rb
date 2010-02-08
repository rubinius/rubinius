def Bench.run
  i = 0
  while @should_run
    # string#end_with_eh(...)
    raise "string#end_with_eh(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
