# -*- encoding: us-ascii -*-

class Thread
  def self.start(*args)
    raise ArgumentError.new("no block passed to Thread.start") unless block_given?

    thr = Rubinius.invoke_primitive :thread_allocate, self

    Rubinius.asm(args, thr) do |args, obj|
      run obj
      dup

      send :setup, 0, true
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
    sleep
    nil
  end

  # Called by Thread#fork in the new thread
  #
  def __run__()
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
          @joins.each { |join| join.send self }
        end
      end
    rescue Exception => e
      # I don't really get this, but this is MRI's behavior. If we're dying
      # by request, ignore any raised exception.
      @exception = e # unless @dying
    ensure
      if Rubinius.thread_state[0] == :thread_kill
        @killed = true
      end
      @alive = false
      Rubinius.unlock(self)
      unlock_locks
    end

    if @exception
      if abort_on_exception or Thread.abort_on_exception
        Thread.main.raise @exception
      elsif $DEBUG
        STDERR.puts "Exception in thread: #{@exception.message} (#{@exception.class})"
      end
    end
  end

  def setup
    @group = nil
    @alive = true
    @result = false
    @exception = nil
    @critical = false
    @dying = false
    @joins = []
    @killed = false
  end

  def kill
    @dying = true
    @sleep = false
    Rubinius.synchronize(self) do
      kill_prim
    end
  end

  alias_method :exit, :kill
  alias_method :terminate, :kill

  def value
    join_inner do
      @killed ? nil : @result
    end
  end

  def active_exception
    nil
  end
end
