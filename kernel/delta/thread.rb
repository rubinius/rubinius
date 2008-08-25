class Thread
  Thread.current.setup(true)
  Thread.initialize_main_thread(Thread.current)
  dg = ThreadGroup.new
  ThreadGroup.const_set :Default, dg
  dg.add Thread.current
end
