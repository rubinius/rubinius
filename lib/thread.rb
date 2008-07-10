#
#		thread.rb - thread support classes
#			$Date: 2006-12-31 07:02:22 -0800 (Sun, 31 Dec 2006) $
#			by Yukihiro Matsumoto <matz@netlab.co.jp>
#
# Copyright (C) 2001  Yukihiro Matsumoto
# Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
# Copyright (C) 2000  Information-technology Promotion Agency, Japan
#

unless defined? Thread
  raise "Thread not available for this ruby interpreter"
end

unless defined? ThreadError
  class ThreadError < StandardError
  end
end

if $DEBUG
  Thread.abort_on_exception = true
end

class Thread
  #
  # Wraps a block in Thread.critical, restoring the original value upon exit
  # from the critical section.
  #
  def Thread.exclusive
    _old = Thread.critical
    begin
      Thread.critical = true
      return yield
    ensure
      Thread.critical = _old
    end
  end
end

class Mutex
  def initialize
    @lock = Channel.new
    @owner = nil
    @waiters = []
    @lock << nil
  end

  def locked?
    @lock.receive
    begin
      !!@owner
    ensure
      @lock << nil
    end
  end

  def try_lock
    @lock.receive
    begin
      if @owner
        false
      else
        @owner = Thread.current
        true
      end
    ensure
      @lock << nil
    end
  end

  def lock
    @lock.receive
    begin
      while @owner
        wchan = Channel.new
        @waiters.push wchan
        @lock << nil
        wchan.receive
        @lock.receive
      end
      @owner = Thread.current
      self
    ensure
      @lock << nil
    end
  end

  def unlock
    @lock.receive
    begin
      raise ThreadError, "Not owner" unless @owner == Thread.current
      @owner = nil
      @waiters.shift << nil unless @waiters.empty?
      self
    ensure
      @lock << nil
    end
  end

  def synchronize
    lock
    begin
      yield
    ensure
      unlock
    end
  end
end

# 
# ConditionVariable objects augment class Mutex. Using condition variables,
# it is possible to suspend while in the middle of a critical section until a
# resource becomes available.
#
# Example:
#
#   require 'thread'
#
#   mutex = Mutex.new
#   resource = ConditionVariable.new
#   
#   a = Thread.new {
#     mutex.synchronize {
#       # Thread 'a' now needs the resource
#       resource.wait(mutex)
#       # 'a' can now have the resource
#     }
#   }
#   
#   b = Thread.new {
#     mutex.synchronize {
#       # Thread 'b' has finished using the resource
#       resource.signal
#     }
#   }
#
class ConditionVariable
  #
  # Creates a new ConditionVariable
  #
  def initialize
    @lock = Channel.new
    @waiters = []
    @lock << nil
  end
  
  #
  # Releases the lock held in +mutex+ and waits; reacquires the lock on wakeup.
  #
  def wait(mutex, timeout=nil)
    @lock.receive
    begin
      wchan = Channel.new
      mutex.unlock
      @waiters.push wchan
      @lock << nil
      if timeout
        timeout_ms = (timeout*1000000).to_i
        timeout_id = Scheduler.send_in_microseconds(wchan, timeout_ms, nil)
      else
        timeout_id = nil
      end
      signaled = wchan.receive
      Scheduler.cancel(timeout_id) if timeout
      mutex.lock
      @lock.receive
      unless signaled or @waiters.delete wchan
        # we timed out, but got signaled afterwards (e.g. while waiting to
        # acquire @lock), so pass that signal on to the next waiter
        @waiters.shift << true unless @waiters.empty?
      end
      if timeout
        !!signaled
      else
        nil
      end
    ensure
      @lock << nil
    end
  end
  
  #
  # Wakes up the first thread in line waiting for this lock.
  #
  def signal
    @lock.receive
    @waiters.shift << true unless @waiters.empty?
    @lock << nil
    nil
  end

  #
  # Wakes up all threads waiting for this lock.
  #
  def broadcast
    @lock.receive
    @waiters.shift << true until @waiters.empty?
    @lock << nil
    nil
  end
end

#
# This class provides a way to synchronize communication between threads.
#
# Example:
#
#   require 'thread'
#   
#   queue = Queue.new
#   
#   producer = Thread.new do
#     5.times do |i|
#       sleep rand(i) # simulate expense
#       queue << i
#       puts "#{i} produced"
#     end
#   end
#   
#   consumer = Thread.new do
#     5.times do |i|
#       value = queue.pop
#       sleep rand(i/2) # simulate expense
#       puts "consumed #{value}"
#     end
#   end
#   
#   consumer.join
#
class Queue
  #
  # Creates a new queue.
  #
  def initialize
    @que = []
    @que.taint		# enable tainted comunication
    self.taint
    @mutex = Mutex.new
    @resource = ConditionVariable.new
    @waiting = 0
  end

  #
  # Pushes +obj+ to the queue.
  #
  def push(obj)
    @mutex.synchronize do
      @que.push obj
      begin        
        @resource.signal
      rescue ThreadError
        retry
      end
    end
  end

  #
  # Alias of push
  #
  alias << push

  #
  # Alias of push
  #
  alias enq push

  #
  # Retrieves data from the queue.  If the queue is empty, the calling thread is
  # suspended until data is pushed onto the queue.  If +non_block+ is true, the
  # thread isn't suspended, and an exception is raised.
  #
  def pop(non_block=false)
    while true
      @mutex.synchronize do
        @waiting -= 1 if @waiting > 0
        if @que.empty?
          raise ThreadError, "queue empty" if non_block
          @waiting += 1
          @resource.wait(@mutex)
        else
          return @que.shift
        end
      end
    end
  end

  #
  # Alias of pop
  #
  alias shift pop

  #
  # Alias of pop
  #
  alias deq pop

  #
  # Returns +true+ if the queue is empty.
  #
  def empty?
    @que.empty?
  end

  #
  # Removes all objects from the queue.
  #
  def clear
    @que.clear
  end

  #
  # Returns the length of the queue.
  #
  def length
    @que.length
  end

  #
  # Alias of length.
  #
  alias size length

  #
  # Returns the number of threads waiting on the queue.
  #
  def num_waiting
    @waiting
  end
end

#
# This class represents queues of specified size capacity.  The push operation
# may be blocked if the capacity is full.
#
# See Queue for an example of how a SizedQueue works.
#
class SizedQueue < Queue
  #
  # Creates a fixed-length queue with a maximum size of +max+.
  #
  def initialize(max)
    raise ArgumentError, "queue size must be positive" unless max > 0
    @max = max
    @queue_wait = []
    @queue_wait.taint		# enable tainted comunication
    super()
  end

  #
  # Returns the maximum size of the queue.
  #
  def max
    @max
  end

  #
  # Sets the maximum size of the queue.
  #
  def max=(max)
    diff = nil
    @mutex.synchronize {
      if max <= @max
        @max = max
      else
        diff = max - @max
        @max = max
      end
    }
    if diff
      diff.times do
	begin
	  t = @queue_wait.shift
	  t.run if t
	rescue ThreadError
	  retry
	end
      end
    end
    max
  end

  #
  # Pushes +obj+ to the queue.  If there is no space left in the queue, waits
  # until space becomes available.
  #
  def push(obj)
    t = nil
    @mutex.synchronize{
      while true
        break if @que.length <= @max
        @queue_wait.push Thread.current
        @mutex.sleep
      end
    
      @que.push obj
      begin
        t = @waiting.shift
        t.wakeup if t
      rescue ThreadError
        retry
      end
    }
    
    begin
      t.run if t
    rescue ThreadError
    end
  end

  #
  # Alias of push
  #
  alias << push

  #
  # Alias of push
  #
  alias enq push

  #
  # Retrieves data from the queue and runs a waiting thread, if any.
  #
  def pop(*args)
    retval = super
    t = nil
    @mutex.synchronize {
      if @que.length < @max
        begin
          t = @queue_wait.shift
          t.wakeup if t
        rescue ThreadError
          retry
        end
      end
    }
    begin
      t.run if t
    rescue ThreadError
    end
    retval
  end

  #
  # Alias of pop
  #
  alias shift pop

  #
  # Alias of pop
  #
  alias deq pop

  #
  # Returns the number of threads waiting on the queue.
  #
  def num_waiting
    @waiting.size + @queue_wait.size
  end
end

# Documentation comments:
#  - How do you make RDoc inherit documentation from superclass?
