# -*- encoding: us-ascii -*-

class Thread
  def self.set_critical(obj)
    Rubinius.primitive :thread_set_critical
    Kernel.raise PrimitiveFailure, "Thread.set_critical failed"
  end

  def self.start(*args)
    thr = Rubinius.invoke_primitive :thread_allocate, self

    Rubinius.asm(args, thr) do |args, obj|
      run obj
      dup

      push_false
      send :setup, 1, true
      pop

      run args
      push_block
      send_with_splat :__thread_initialize__, 0, true
      # no pop here, as .asm blocks imply a pop as they're not
      # allowed to leak a stack value
    end

    return thr
  end

  class << self
    alias_method :fork, :start
  end

  def self.stop
    # Make sure that if we're stopping the current Thread,
    # others can run, so reset critical.
    Thread.critical = false
    sleep
    nil
  end

  def self.critical
    @critical
  end

  def self.critical=(value)
    set_critical value
    @critical = !!value
  end
end
