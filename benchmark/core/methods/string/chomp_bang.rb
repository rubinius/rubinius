def Bench.run
  i = 0
  while @should_run
    # string#chomp_bang(...)
    raise "string#chomp_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
