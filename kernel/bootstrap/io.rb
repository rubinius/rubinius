class IO

  class Buffer
    def self.allocate
      Ruby.primitive :iobuffer_allocate
      raise PrimitiveFailure, "IO::Buffer.allocate primitive failed"
    end

    ##
    # Returns the number of bytes that could be written to the buffer.
    # If the number is less then the expected, then we need to +empty_to+
    # the IO, and +unshift+ again beginning at +start_pos+.
    def unshift(str, start_pos)
      Ruby.primitive :iobuffer_unshift
      raise PrimitiveFailure, "IO::Buffer#unshift primitive failed"
    end

    def fill(io)
      Ruby.primitive :iobuffer_fill

      unless io.kind_of? IO
        return fill(io.to_io)
      end

      raise PrimitiveFailure, "IOBuffer#fill primitive failed"
    end
  end

  def self.allocate
    Ruby.primitive :io_allocate
    raise PrimitiveFailure, "IO.allocate primitive failed"
  end

  def self.open_with_mode(path, mode, perm)
    Ruby.primitive :io_open
    raise PrimitiveFailure, "IO.open_with_mode primitive failed"
  end

  def self.connect_pipe(lhs, rhs)
    Ruby.primitive :io_connect_pipe
    raise PrimitiveFailure, "IO.connect_pipe primitive failed"
  end

  def self.select_primitive(readables, writables, errorables, timeout)
    Ruby.primitive :io_select
    raise PrimitiveFailure, "IO#select_primitive primitive failed"
  end

  def self.fnmatch(pattern, path, flags)
    Ruby.primitive :io_fnmatch
    raise PrimitiveFailure, "IO#fnmatch primitive failed"
  end

  # Instance primitive bindings

  def ensure_open
    Ruby.primitive :io_ensure_open
    raise PrimitiveFailure, "IO#ensure_open primitive failed"
  end

  def read_primitive(number_of_bytes)
    Ruby.primitive :io_sysread
    raise PrimitiveFailure, "IO::sysread primitive failed!"
  end

  def write(str)
    Ruby.primitive :io_write
    raise PrimitiveFailure, "IO#write failed. Might not have passed a string."
  end

  def blocking_read(size)
    Ruby.primitive :io_blocking_read
    raise PrimitiveFailure, "IO#blocking_read primitive failed"
  end

  def read_if_available(size)
    Ruby.primitive :io_read_if_available
    raise PrimitiveFailure, "IO#read_if_available primitive failed"
  end

  def raw_write(str)
    Ruby.primitive :io_write_nonblock
    raise PrimitiveFailure, "IO#write_nonblock primitive failed"
  end

  def reopen_io(other)
    Ruby.primitive :io_reopen
    raise ArgumentError, "IO#prim_reopen only accepts an IO object"
  end

  def reopen_path(string, mode)
    Ruby.primitive :io_reopen_path

    if mode.kind_of? Bignum
      raise ArgumentError, "Bignum too big for mode"
    end

    reopen_path StringValue(string), Integer(mode)
  end

  def prim_seek(amount, whence)
    Ruby.primitive :io_seek
    raise RangeError, "#{amount} is too big"
  end

  def query(which)
    Ruby.primitive :io_query
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
    Ruby.primitive :io_close
    raise PrimitiveFailure, "IO#close primitive failed"
  end

  #
  # Close read and/or write stream of a full-duplex descriptor.
  #
  # @todo   More documentation. Much more. --rue
  #
  def shutdown(how)
    Ruby.primitive :io_shutdown
    raise PrimitiveFailure, "IO#shutdown primitive failed"
  end

  def socket_recv(bytes, flags, type)
    Ruby.primitive :io_socket_read
    raise PrimitiveFailure, "io_socket_read failed"
  end

  module Socketable
    def accept
      Ruby.primitive :io_accept
      raise PrimitiveFailure, "io_accept failed"
    end
  end

end
