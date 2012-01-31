#
#		thread.rb - thread support classes
#			$Date: 2006-12-31 07:02:22 -0800 (Sun, 31 Dec 2006) $
#			by Yukihiro Matsumoto <matz@netlab.co.jp>
#
# Copyright (C) 2001  Yukihiro Matsumoto
# Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
# Copyright (C) 2000  Information-technology Promotion Agency, Japan
#

if $DEBUG
  Thread.abort_on_exception = true
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
    @waiters = []
  end

  #
  # Releases the lock held in +mutex+ and waits; reacquires the lock on wakeup.
  #
  def wait(mutex, timeout=nil)
    Rubinius.lock(self)

    begin
      wchan = Rubinius::Channel.new

      begin
        mutex.unlock
        @waiters.push wchan
        Rubinius.unlock(self)
        signaled = wchan.receive_timeout timeout
      ensure
        mutex.lock
        Rubinius.lock(self)

        unless signaled or @waiters.delete(wchan)
          # we timed out, but got signaled afterwards (e.g. while waiting to
          # acquire @lock), so pass that signal on to the next waiter
          @waiters.shift << true unless @waiters.empty?
        end
      end

      if timeout
        !!signaled
      else
        self
      end
    ensure
      Rubinius.unlock(self)
    end
  end

  #
  # Wakes up the first thread in line waiting for this lock.
  #
  def signal
    Rubinius.lock(self)
    begin
      @waiters.shift << true unless @waiters.empty?
    ensure
      Rubinius.unlock(self)
    end
    self
  end

  #
  # Wakes up all threads waiting for this lock.
  #
  def broadcast
    Rubinius.lock(self)
    begin
      @waiters.shift << true until @waiters.empty?
    ensure
      Rubinius.unlock(self)
    end
    self
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
    @waiting = []
    @waiting.taint
    @mutex = Mutex.new
    @resource = ConditionVariable.new
  end

  #
  # Pushes +obj+ to the queue.
  #
  def push(obj)
    @mutex.synchronize do
      @que.push obj
      @resource.signal
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
        #FIXME: some code in net or somewhere violates encapsulation
        #and demands that a waiting queue exist for Queue, as a result
        #we have to do a linear search here to remove the current Thread.
        @waiting.delete(Thread.current)
        if @que.empty?
          raise ThreadError, "queue empty" if non_block
          @waiting.push Thread.current
          @resource.wait(@mutex)
        else
          retval = @que.shift
          @resource.signal
          return retval
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
    @waiting.size
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
    @size_mutex = Mutex.new
    @sem = ConditionVariable.new
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
    @size_mutex.synchronize do
      @max = max
      @sem.broadcast(@size_mutex)
    end
    max
  end

  #
  # Pushes +obj+ to the queue.  If there is no space left in the queue, waits
  # until space becomes available.
  #
  def push(obj)
    while true
      @size_mutex.synchronize do
        @queue_wait.delete(Thread.current)
        if @que.size >= @max
          @queue_wait.push Thread.current
          @sem.wait(@size_mutex)
        else
          return super(obj)
        end
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
  # Retrieves data from the queue and runs a waiting thread, if any.
  #
  def pop(*args)
    retval = super
    
    @size_mutex.synchronize do
      if @que.size < @max
        @sem.broadcast
      end      
    end
    
    return retval
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
