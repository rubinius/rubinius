def Bench.run
  i = 0
  str = "hellohellohellohellohellohellohello"
  while @should_run
    str.delete("lo")
    i += 1
  end

  @iterations = i
end
