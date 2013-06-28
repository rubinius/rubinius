# -*- encoding: us-ascii -*-

class Thread
  Thread.initialize_main_thread(Thread.current)
  dg = ThreadGroup::Default
  Default = dg
  dg.add Thread.current
end
