ready = false

t = Thread.new do
  Thread.pass until ready
  raise RuntimeError, "exception from Thread"
end

t.abort_on_exception = true
ready = true

sleep 1
