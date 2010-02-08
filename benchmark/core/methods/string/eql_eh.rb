def Bench.run
  i = 0
  while @should_run
    # string#eql_eh(...)
    raise "string#eql_eh(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
