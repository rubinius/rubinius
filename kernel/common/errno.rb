##
# Interface to the C errno integer.

module Errno

  ##
  # Raises the appropriate SystemCallError exception with +additional+ as the
  # message.  Equivalent to MRI's rb_sys_fail().
  #
  # Unlike rb_sys_fail(), handle does not raise an exception if errno is 0.

  def self.handle(additional = nil)
    err = FFI::Platform::POSIX.errno
    return if err == 0

    exc = Errno::Mapping[err]
    if exc
      msg = FFI::Platform::POSIX.strerror(err)

      if additional
        msg << " - " << additional
      end

      raise exc.new(msg, err)
    else
      raise "Unknown error: #{FFI::Platform::POSIX.strerror(err)} (#{err})"
    end
  end
end
