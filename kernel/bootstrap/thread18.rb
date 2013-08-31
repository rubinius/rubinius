# -*- encoding: us-ascii -*-

class Thread
  def self.set_critical(obj)
    Rubinius.primitive :thread_set_critical
    Kernel.raise PrimitiveFailure, "Thread.set_critical primitive failed"
  end

  def self.start(*args)
    thr = Rubinius.invoke_primitive :thread_allocate, self

    Rubinius.asm(args, thr) do |args, obj|
      run obj
      dup

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

  # Called by Thread#fork in the new thread
  #
  def __run__
    exception = nil
    begin
      begin
        Rubinius.unlock(self)
        @result = @block.call(*@args)
      ensure
        begin
          # We must lock self in a careful way.
          #
          # At this point, it's possible that an other thread does Thread#raise
          # and then our execution is interrupted AT ANY GIVEN TIME. We
          # absolutely must make sure to lock self as soon as possible to lock
          # out interrupts from other threads.
          #
          # Rubinius.uninterrupted_lock(self) just does that.
          #
          # Notice that this can't be moved to other methods and there should be
          # no preceding code before it in the enclosing ensure clause.
          # These are to prevent any interrupted lock failures.
          Rubinius.uninterrupted_lock(self)

          # Now, we locked self. No other thread can interrupt this thread
          # anymore.
          # If there is any not-triggered interrupt, check and process it. In
          # either case, we jump to the following ensure clause.
          Rubinius.check_interrupts
        ensure
          unlock_locks
          @joins.each { |join| join.send self }
        end
      end
    rescue Exception => e
      # I don't really get this, but this is MRI's behavior. If we're dying
      # by request, ignore any raised exception.
      exception = e # unless @dying
    ensure
      unless exception && (abort_on_exception || Thread.abort_on_exception)
        @exception = exception
        if $DEBUG
          STDERR.puts "Exception in thread: #{@exception.message} (#{@exception.class})"
        end
      end
      @alive = false
      Rubinius.unlock(self)
    end

    unless @exception
      Thread.main.raise exception
    end
  end

  def kill
    @dying = true
    Rubinius.synchronize(self) do
      if @sleep and @killed
        @sleep = false
        wakeup
      else
        @sleep = false
        @killed = true
        kill_prim
      end
    end
  end

  alias_method :exit, :kill
  alias_method :terminate, :kill

  def value
    join_inner { @result }
  end

  def active_exception
    current_exception
  end
end
