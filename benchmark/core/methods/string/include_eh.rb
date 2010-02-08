def Bench.run
  i = 0
  while @should_run
    # string#include_eh(...)
    raise "string#include_eh(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
