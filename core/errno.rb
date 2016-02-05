##
# Interface to the C errno integer.

module Errno
  ##
  # Raises the appropriate SystemCallError exception with +additional+ as the
  # message.  Equivalent to MRI's rb_sys_fail().
  #
  # Unlike rb_sys_fail(), handle does not raise an exception if errno is 0.

  def self.handle(additional = nil)
    err = errno
    return if err == 0

    raise SystemCallError.new(additional, err)
  end
  
  def self.errno
    FFI.errno
  end
  
  def self.eql?(code)
    FFI.errno == code
  end

  def self.raise_waitreadable(message=nil)
    raise IO::EAGAINWaitReadable, message
  end

  def self.raise_waitwritable(message=nil)
    raise IO::EAGAINWaitWritable, message
  end

  def self.raise_eagain(message=nil)
    raise_errno(message, Errno::EAGAIN::Errno)
  end

  def self.raise_errno(message, errno)
    raise SystemCallError.new(message, errno)
  end
  private :raise_errno
end
