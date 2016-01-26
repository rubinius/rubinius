class IO

  def finalizer(io)
    return unless io.descriptor
    return if io.descriptor == -1

    # FIXME: bunch of stuff about flushing if there are unwritten bytes.
    # Not sure what to do until I figure out all of hte mmap stuff.

    # don't close stdin, stdout or stderr (file descriptors 0, 1 and 2)
    if io.descriptor > 2 # FIXME: should use a constant here instead of 2?
      # FIXME: more stuff I don't quite understand until mmap stuff is figured out
    end
  end

  def self.finalizer(io)
    Proc.new do
      io.finalizer(io)
    end
  end

  def self.fnmatch(pattern, path, flags)
    Rubinius.primitive :io_fnmatch
    raise PrimitiveFailure, "IO#fnmatch primitive failed"
  end

  #
  # Close read and/or write stream of a full-duplex descriptor.
  #
  # @todo   More documentation. Much more. --rue
  #
  def shutdown(how)
    Rubinius.primitive :io_shutdown
    raise PrimitiveFailure, "IO#shutdown primitive failed"
  end

  def socket_recv(bytes, flags, type)
    Rubinius.primitive :io_socket_read
    raise PrimitiveFailure, "io_socket_read failed"
  end

  module TransferIO
    def send_io
      Rubinius.primitive :io_send_io
      raise PrimitiveFailure, "IO#send_io failed"
    end

    def recv_fd
      Rubinius.primitive :io_recv_fd
      raise PrimitiveFailure, "IO#recv_fd failed"
    end
  end

end
