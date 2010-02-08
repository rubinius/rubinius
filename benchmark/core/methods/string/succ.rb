def Bench.run
  i = 0
  while @should_run
    # string#succ(...)
    raise "string#succ(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
