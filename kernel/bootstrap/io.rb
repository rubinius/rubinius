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

  # FIXME: skipped #sysread

  # FIXME: skipped #read_if_available since it depends on #select which is unfinished

  #  def self.allocate
  #    Rubinius.primitive :io_allocate
  #    raise PrimitiveFailure, "IO.allocate primitive failed"
  #  end
  #
  #  def self.open_with_mode(path, mode, perm)
  #    Rubinius.primitive :io_open
  #    raise PrimitiveFailure, "IO.open_with_mode primitive failed"
  #  end

  def self.connect_pipe(lhs, rhs)
    Rubinius.primitive :io_connect_pipe
    raise PrimitiveFailure, "IO.connect_pipe primitive failed"
  end

  def self.select_primitive(readables, writables, errorables, timeout)
    Rubinius.primitive :io_select
    raise IOError, "Unable to select on IO set (descriptor too big?)"
  end

  def self.fnmatch(pattern, path, flags)
    Rubinius.primitive :io_fnmatch
    raise PrimitiveFailure, "IO#fnmatch primitive failed"
  end

  # Instance primitive bindings

  #  def ensure_open
  #    Rubinius.primitive :io_ensure_open
  #    raise PrimitiveFailure, "IO#ensure_open primitive failed"
  #  end

  #  def read_primitive(number_of_bytes)
  #    Rubinius.primitive :io_sysread
  #    raise PrimitiveFailure, "IO::sysread primitive failed"
  #  end

  def write2(str)
    Rubinius.primitive :io_write
    raise PrimitiveFailure, "IO#write primitive failed"
  end

#  def read_if_available(size)
#    Rubinius.primitive :io_read_if_available
#    raise PrimitiveFailure, "IO#read_if_available primitive failed"
#  end

#  def raw_write(str)
#    Rubinius.primitive :io_write_nonblock
#    raise PrimitiveFailure, "IO#write_nonblock primitive failed"
#  end

  def reopen_io(other)
    Rubinius.primitive :io_reopen
    raise ArgumentError, "IO#prim_reopen only accepts an IO object"
  end

  def reopen_path(string, mode)
    Rubinius.primitive :io_reopen_path

    if mode.kind_of? Bignum
      raise ArgumentError, "Bignum too big for mode"
    end

    reopen_path StringValue(string), Integer(mode)
  end

  #  def prim_seek(amount, whence)
  #    Rubinius.primitive :io_seek
  #    raise RangeError, "#{amount} is too big"
  #  end

  def self.prim_truncate(name, offset)
    Rubinius.primitive :io_truncate
    raise RangeError, "#{offset} is too big"
  end

  def prim_ftruncate(offset)
    Rubinius.primitive :io_ftruncate
    raise RangeError, "#{amount} is too big"
  end

  def query(which)
    Rubinius.primitive :io_query
    raise PrimitiveFailure, "IO#query primitive failed"
  end

  def reopen(other)
    reopen_io other
  end

  def tty?
    query :tty?
  end

  def ttyname
    query :ttyname
  end

  def close
    Rubinius.primitive :io_close
    raise PrimitiveFailure, "IO#close primitive failed"
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

  module Socketable
    def accept
      Rubinius.primitive :io_accept
      raise PrimitiveFailure, "io_accept failed"
    end
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
