# The Schedule class provides an interface to the VM, instructing
# the VM about an operation to be performed. The operation is performed
# in the background, with respect to running ruby code.
#
# This means that all methods of Scheduler return right away. When
# the operation has completed, the +chan+ argument, which is a Channel
# object, is sent a value. See channel.rb for more details on what happens
# in that case, but the simple explanation is that a thread that is waiting
# on the Channel for a value is woken up and started.
#
# Using this setup, we can easily write thread aware code, that doesn't
# block the entire process, only blocking a single ruby Thread.
#
# == Where should I use calls to Scheduler? ==
#
# Because Rubinius uses green threads, it has full control over them. But
# that also means that, for threads to work properly, we must never perform
# a C blocking operation, because it will block all Threads, not just the
# current one. Because we expose C functions directly via FFI, this is
# a very important rule to follow.
#
# A quick example is Socket#accept. When it's called, the expected
# behavior is that the current Thread blocks until a new connection
# is made. Remember that Socket#accept is, at it's core, implemented
# with accept(2), which is a blocking C function. We therefore must
# never call accept(2) unless we know it's not going to block. We
# find that out using the Scheduler. Here is a very simple version:
#
# class Socket
#   def simple_accept
#     chan = Channel.new
#     Scheduler.send_on_readable chan, self, nil, nil
#     chan.receive
#     # Now we know that there is data on +self+, which means
#     # that there is a new connection. We can now call accept(2)
#     # safely because we know it wont block.
#     fd = Socket::Foreign.accept self, ...
#
#     return Socket.from_fd(fd)
#   end
# end
#

class Scheduler
  # In +microseconds+ microseconds, send the object +tag+ to the
  # Channel +chan+. This is used to implement sleep. For example:
  #
  # chan = Channel.new
  # Scheduler.send_in_microseconds chan, 1000, :hello
  # obj = chan.receive
  #      The chan.receive appears to block the current thread, waking
  #      up when there is a value on the channel. So, to the caller of this
  #      code, it appears that they have slept for 1000 seconds.
  # p obj # => :hello
  #
  # Returns an event id, for use with #cancel
  #
  def self.send_in_microseconds(chan, microseconds, tag)
    Ruby.primitive :channel_send_in_microseconds
    raise PrimitiveFailure, "primitive failed"
  end

  # Same as send_in_microseconds, but the 2nd argument is seconds, not
  # microseconds.
  def self.send_in_seconds(chan, seconds, tag)
    Ruby.primitive :channel_send_in_seconds
    raise PrimitiveFailure, "primitive failed"
  end

  # Instructs the VM to send a value to +chan+ when the IO object
  # +io+ is readable. +buffer+ and +nbytes+ are optional, and should
  # be set to nil if you don't wish to use them. There are 2 modes of
  # operation, depending on if +buffer+ is nil or not.
  #
  # If +buffer+ is nil, +chan+ is sent the internal file descriptor
  # number of +io+. This can be used to wait on multiple IO objects
  # with one channel, and then, given the return value of #receive
  # figure out which IO object was ready.
  #
  # The second mode is when +buffer+ is an IO::Buffer object. In
  # this mode, when +io+ becomes readable, +nbytes+ bytes are read
  # from IO and stored into +buffer+. Note that this is done using
  # the read(2) C function, and thus +nbytes+ is the maximum number
  # of bytes that will be read, not the actual number read. The
  # value send to the channel is the actual number of bytes read as
  # a Fixnum object.
  #
  # If an error occurs while reading in the 2nd mode, the value
  # send to the channel is a subclass of SystemCallError, which
  # indicates the errno present when read(2) was called.
  #
  # See IO#sysread in kernel/core/io.rb for a simple example.
  #
  # Returns an event id, for use with #cancel
  #
  def self.send_on_readable(chan, io, buffer, nbytes)
    Ruby.primitive :channel_send_on_readable
    raise PrimitiveFailure, "primitive failed"
  end

  # Instruct the VM to send +nil+ to +chan+ when IO +io+
  # is writable.
  #
  # Returns an event id, for use with #cancel
  #
  def self.send_on_writable(chan, io)
    Ruby.primitive :channel_send_on_readable
    raise PrimitiveFailure, "primitive failed"
  end

  # Instruct the VM to send a Thread object to +chan+ when
  # unix signal +signum+ (a Fixnum) is received by the process.
  #
  # The Thread object that will be sent is the Thread object that was
  # running when the unix signal was received by the process.
  #
  # Example:
  #
  # chan = Channel.new
  # Scheduler.send_on_signal chan, 2 # 2 is SIGINT
  # thr = Thread.new { some_long_operation }
  # val = chan.receive # blocks Thread.current until SIGINT is received
  #                    # => #<Thread:0x....>
  # p val == thr       # => true
  #
  # Unlike all other +send_on+ methods, this one is persistant. Meaning
  # that calling +send_on_signal+ once for SIGINT will cause all
  # future times SIGINT is seen to send a value to +chan+.
  #
  # Returns an event id, for use with #cancel
  #
  def self.send_on_signal(chan, signum)
    Ruby.primitive :channel_send_on_signal
    raise PrimitiveFailure, "primitive failed"
  end

  # The true interface to the primitive. +send_on_stopped+ simply
  # provides some default values for +pid+ and +flags+
  #
  def self.send_on_stopped_prim(chan, pid, flags)
    Ruby.primitive :channel_send_on_stopped
    raise PrimitiveFailure, "primitive failed"
  end

  # Instructs the VM to send a value to +chan+ when process +pid+
  # is no longer running. This is used to implement Process.wait.
  # Flags is a fixnum, currently accepting a single flag:
  #   Process::WNOHANG (value 1), to indicate that send_on_stopped
  #   shouldn't block, but send nil if the pid is still running.
  #
  # The sent value is 1 of 3 types:
  #   A tuple containing <pid, exit_status>
  #   false if there is no such process called +pid+
  #   nil if flags indicated nohang, meaning that the pid is valid, but
  #       not stopped currently.
  #
  # Returns an event id, for use with #cancel
  #
  def self.send_on_stopped(chan, pid=-1, flags=0)
    send_on_stopped_prim(chan, pid, flags)
  end

  # All +send_on+ methods return a Fixnum, representing an event
  # id that was just registered. That id can be passed to #cancel
  # to inform the VM that we're not longer interested in that event.
  # No value is sent to the channel that was registered.
  #
  def self.cancel(id)
    Ruby.primitive :scheduler_cancel
    raise PrimitiveFailure, "primitive failed"
  end
end

