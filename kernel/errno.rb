##
# Interface to the C errno integer.

module Errno
  FFI = Rubinius::FFI

  ##
  # Raises the appropriate SystemCallError exception with +additional+ as the
  # message.  Equivalent to MRI's rb_sys_fail().
  #
  # Unlike rb_sys_fail(), handle does not raise an exception if errno is 0.

  def self.handle(additional = nil)
    err = FFI::Platform::POSIX.errno
    return if err == 0

    raise SystemCallError.new(additional, err)
  end
end
