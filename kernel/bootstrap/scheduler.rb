##
# The Scheduler provides an interface to the VM, allowing the VM to tell ruby
# when various operations have completed.  The operation is performed in the
# background with respect to running ruby code.
#
# All methods of Scheduler return right away.  When the operation has
# completed, the Channel is sent a value.  Using this setup we can easily
# write thread aware code that doesn't block the entire process, only
# a single ruby Thread.
#
# == Where should I use calls to Scheduler?
#
# Rubinius uses green threads, so it must never perform a blocking C call as
# that will block all ruby threads.
#
# In the case of IO#read, the C read(2) will block until enough bytes are
# ready to be read.  Instead of directly calling read(2), send_on_readable is
# called and waits in the VM until the file descriptor is ready to be read.

module Rubinius
  class Scheduler

    ##
    # In +microseconds+ send the +tag+ to +channel+.  Returns an event id for
    # use with #cancel.  This is used to implement sleep.  For example:
    #
    #   chan = Channel.new
    #   Scheduler.send_in_microseconds chan, 1000, :hello
    #   p chan.receive
    #   # 1 second later :hello is printed
    #
    # The chan.receive appears to block the current thread, waking up when there
    # is a value on the channel.  To the caller of this code it appears that
    # they have slept for 1 seconds.

    def self.send_in_microseconds(channel, microseconds, tag)
      Ruby.primitive :scheduler_send_in_microseconds
      raise PrimitiveFailure, "Scheduler.send_in_microseconds primitive failed"
    end

    ##
    # Same as send_in_microseconds, but the 2nd argument is seconds, not
    # microseconds.

    def self.send_in_seconds(chan, seconds, tag)
      Ruby.primitive :scheduler_send_in_seconds
      raise PrimitiveFailure, "Scheduler.send_in_seconds primitive failed"
    end

    ##
    # Instructs the VM to send a value to +channel+ when the IO object +io+ is
    # readable.  +buffer+ and +nbytes+ are optional, and should be set to nil if
    # you don't wish to use them.  Returns an event id, for use with #cancel
    #
    # There are 2 modes of operation, depending on if +buffer+ is nil or not.
    #
    # If +buffer+ is nil the +channel+ is sent the file descriptor for +io+.
    # This can be used to wait on multiple IO objects with one channel and
    # figure out which IO object was ready given the return value of #receive3
    #
    # The second mode is when +buffer+ is an IO::Buffer object.  In this mode
    # when +io+ becomes readable +nbytes+ bytes are read from IO and stored into
    # +buffer+.  Note that this is done using the read(2) C function and thus
    # +nbytes+ is the maximum number of bytes that will be read, not the actual
    # number read.  The value sent to the channel is the actual number of bytes
    # read as a Fixnum object.
    #
    # If an error occurs while reading in the 2nd mode, the value sent to the
    # channel is a SystemCallError representing the failure in read(2).
    #
    # See IO#sysread in kernel/common/io.rb for a simple example.

    def self.send_on_readable(chan, io, buffer, nbytes)
      Ruby.primitive :scheduler_send_on_readable
      raise PrimitiveFailure, "Scheduler.send_on_readable primitive failed"
    end

    ##
    # Instructs the VM to send the file descriptor for +io+ to +channel+
    # when +io+ is writable. Returns an event id, for use with #cancel

    def self.send_on_writable(chan, io)
      Ruby.primitive :scheduler_send_on_writable
      raise PrimitiveFailure, "Scheduler.send_on_writable primitive failed"
    end

    ##
    # Instructs the VM to send a Thread object to +chan+ when UNIX signal
    # +signum+, a Fixnum, is received by the process.  Returns an event id, for
    # use with #cancel
    #
    # The Thread object that will be sent is the Thread object that was running
    # when the signal was received by the process.
    #
    # Example:
    #
    #   chan = Channel.new
    #   Scheduler.send_on_signal chan, 2 # 2 is SIGINT
    #   thr = Thread.new { some_long_operation }
    #   val = chan.receive # blocks Thread.current until SIGINT is received
    #                      # => #<Thread:0x....>
    #   p val == thr       # => true
    #
    # Unlike all other +send_on+ methods, this one is persistant.  Calling
    # +send_on_signal+ once for SIGINT will cause all future SIGINTs to send a
    # value to +chan+.

    def self.send_on_signal(channel, signum)
      Ruby.primitive :scheduler_send_on_signal
      raise PrimitiveFailure, "Scheduler.send_on_signal primitive failed"
    end

    ##
    # Wrapper for the primitive, use +send_on_stopped+
    #
    def self.send_on_stopped_prim(channel, pid, flags)
      Ruby.primitive :scheduler_send_on_stopped
      raise PrimitiveFailure, "Scheduler.send_on_stopped_prim primitive failed"
    end

    ##
    # Instructs the VM to send a value to +channel+ when process +pid+ is no
    # longer running.  This is used to implement Process.wait.  Returns an event
    # id for use with #cancel.
    #
    # Flags is a Fixnum and currently accepts only Process::WNOHANG to indicate
    # send_on_stopped should send nil if the process is still running instead of
    # blocking.
    #
    # The value sent to the channel is either:
    # * A tuple containing +pid+ and +exit_status+
    # * +false+ if there is no process +pid+
    # * nil if flags indicated NOHANG

    def self.send_on_stopped(channel, pid=-1, flags=0)
      send_on_stopped_prim(channel, pid, flags)
    end

    ##
    # All +send_on+ methods return an event id for the registered event.  That
    # id can be passed to #cancel to inform the VM that we're not longer
    # interested in the event.  No value is sent to the channel that was
    # registered.

    def self.cancel(id)
      Ruby.primitive :scheduler_cancel
      raise PrimitiveFailure, "Scheduler.cancel primitive failed"
    end

  end
end
