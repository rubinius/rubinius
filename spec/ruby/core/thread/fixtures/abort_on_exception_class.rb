Thread.abort_on_exception = true

t = Thread.new do
  raise RuntimeError, "exception from Thread"
end

sleep 1
