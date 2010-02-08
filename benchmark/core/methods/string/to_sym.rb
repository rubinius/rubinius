def Bench.run
  i = 0
  while @should_run
    # string#to_sym(...)
    raise "string#to_sym(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
