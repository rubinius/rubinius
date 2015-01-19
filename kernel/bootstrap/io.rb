class IO
  #@@max_descriptors = 2 #Rubinius::AtomicReference.new(2)

  # Import platform constants

  SEEK_SET = Rubinius::Config['rbx.platform.io.SEEK_SET']
  SEEK_CUR = Rubinius::Config['rbx.platform.io.SEEK_CUR']
  SEEK_END = Rubinius::Config['rbx.platform.io.SEEK_END']

  F_GETFL  = Rubinius::Config['rbx.platform.fcntl.F_GETFL']
  F_SETFL  = Rubinius::Config['rbx.platform.fcntl.F_SETFL']

  # O_ACCMODE is /undocumented/ for fcntl() on some platforms
  ACCMODE  = Rubinius::Config['rbx.platform.fcntl.O_ACCMODE']

  F_GETFD  = Rubinius::Config['rbx.platform.fcntl.F_GETFD']
  F_SETFD  = Rubinius::Config['rbx.platform.fcntl.F_SETFD']
  FD_CLOEXEC = Rubinius::Config['rbx.platform.fcntl.FD_CLOEXEC']
  O_CLOEXEC = Rubinius::Config['rbx.platform.file.O_CLOEXEC']


  attr_accessor :descriptor
  attr_accessor :mode

  def initialize(fd)
    @descriptor = fd
    acc_mode = FFI::Platform::POSIX.fcntl(fd, F_GETFL, 0)

    if acc_mode < 0
      # Assume it's closed.
      if Errno.eql?(Errno::EBADF)
        @descriptor = -1
      end

      @mode = nil
    else
      @mode = acc_mode
    end

    #io->ibuffer(state, IOBuffer::create(state));
    @ibuffer = InternalBuffer.new
    @sync = true
    @lineno = 0
    
    # Discover final size of file so we can set EOF properly
    @total_size = sysseek(0, SEEK_END)
    @offset = sysseek(0)

    # Don't bother to add finalization for stdio
    if fd >= 3
      # finalize
    end
  end
  
  def self.initialize_pipe
    obj = allocate
    obj.instance_variable_set :@descriptor, nil
    obj.instance_variable_set :@mode, nil
    obj.instance_variable_set :@eof, false
    obj.instance_variable_set :@lineno, 0
    obj.instance_variable_set :@offset, 0
    
    # setup finalization for pipes
    
    obj
  end
  
  def self.pagesize
    @pagesize ||= FFI::Platform::POSIX.getpagesize
  end

  def self.open_with_mode(path, mode, perm)
    fd = -1
    fd = open_with_cloexec(path, mode, perm)

    if fd < 0
      Errno.handle("failed to open file")
    end

    fd
  end

  def self.open_with_cloexec(path, mode, perm)
    if O_CLOEXEC
      fd = FFI::Platform::POSIX.open(path, mode | O_CLOEXEC, perm)
      update_max_fd(fd)
    else
      fd = FFI::Platform::POSIX.open(path, mode, perm)
      new_open_fd(fd)
    end

    return fd
  end

  def self.new_open_fd(new_fd)
    if new_fd > 2
      flags = FFI::Platform::POSIX.fcntl(new_fd, F_GETFD)
      Errno.handle("fcntl(2) failed") if flags == -1
      flags = FFI::Platform::POSIX.fcntl(new_fd, F_SETFD, FFI::Platform::POSIX.fcntl(new_fd, F_GETFL) | O_CLOEXEC)
      Errno.handle("fcntl(2) failed") if flags == -1
    end

    update_max_fd(new_fd)
  end

  def self.update_max_fd(new_fd)
    #@@max_descriptors.get_and_set(new_fd)
    #@@max_descriptors = @@max_descriptors > fd ? @@max_descriptors : fd
  end

  def reopen(other_fd)
    current_fd = @descriptor

    if FFI::Platform::POSIX.dup2(otherfd, current_fd) == -1
      Errno.handle("reopen")
      return nil
    end

    set_mode

    return true
  end

  def self.reopen_path(path, mode)
    current_fd = @descriptor

    other_fd = -1
    other_fd = open_with_cloexec(path, mode, 0666)

    Exception::errno_error("could not reopen path", Errno.errno, "reopen_path") if other_fd < 0

    if FFI::Platform::POSIX.dup2(other_fd, current_fd) == -1
      if Errno.eql?(Errno::EBADF)
        # means current_fd is closed, so set ourselves to use the new fd and continue
        self.descriptor = other_fd
      else
        FFI::Platform::POSIX.close(other_fd) if other_fd > 0
        Exception::errno_error("could not reopen path", Errno.errno, "reopen_path")
      end
    else
      FFI::Platform::POSIX.close(other_fd)
    end

    set_mode
    return true
  end

  def ensure_open
    if descriptor.nil?
      raise IOError, "uninitialized stream"
    elsif descriptor == -1
      raise IOError, "closed stream"
    elsif descriptor == -2
      raise IOError, "shutdown stream"
    end
    return nil
  end

  def connect_pipe(lhs, rhs)
    fds = [0, 0]

    Errno.handle("creating pipe failed") if pipe(fds) == -1

    new_open_fd(fds[0])
    new_open_fd(fds[1])

    lhs.descriptor = fds[0]
    rhs.descriptor = fds[1]
    lhs.mode = O_RDONLY
    rhs.mode = O_WRONLY
    return true
  end

  def sysseek(offset, whence=SEEK_SET)
    ensure_open

    # FIXME: check +amount+ to make sure it isn't too large

    position = FFI::Platform::POSIX.lseek(descriptor, offset, whence)

    Errno.handle("seek failed") if position == -1

    @offset = position
    return position
  end

  def ftruncate(offset)
    ensure_open

    # FIXME: fail if +offset+ is too large, see C++ code

    status = FFI::Platform::POSIX.ftruncate(descriptor, offset)
    Errno.handle("ftruncate(2) failed") if status == -1
    return status
  end

  def truncate(name, offset)
    # FIXME: fail if +offset+ is too large, see C++ code

    status = FFI::Platform::POSIX.truncate(name, offset)
    Errno.handle("truncate(2) failed") if status == -1
    return status
  end

  def close
    ensure_open
    fd = descriptor
    if fd != -1
      ret_code = FFI::Platform::POSIX.close(fd)

      if ret_code == -1
        Errno.handle("close failed")
      elsif ret_code == 0
        # no op
      else
        raise IOError, "::close(): Unknown error on fd #{fd}"
      end
    end

    self.descriptor = -1

    return nil
  end

  #  /**
  #   *  This is NOT the same as close().
  #   *
  #   *  @todo   Need to build the infrastructure to be able to only
  #   *          remove read or write waiters if a partial shutdown
  #   *          is requested. --rue
  #   */
  def shutdown(how)
    ensure_open
    fd = descriptor

    if how != SHUT_RD && how != SHUT_WR && how != SHUT_RDWR
      raise ArgumentError, "::shutdown(): Invalid `how` #{how} for fd #{fd}"
    end

    ret_code = FFI::Platform::POSIX.shutdown(fd, how)

    if ret_code == -1
      Errno.handle("shutdown(2) failed")
    elsif ret_code == 0
      if how == SHUT_RDWR
        close
        self.descriptor = -2
      end
    else
      Errno.handle("::shutdown(): Unknown error on fd #{fd}")
    end

    return how
  end

  def set_mode
    if F_GETFL
      acc_mode = FFI::Platform::POSIX.fcntl(@descriptor, F_GETFL)
      Ernno.handle("failed") if acc_mode < 0
    else
      acc_mode = 0
    end

    @mode = acc_mode
  end

  def force_read_only
    @mode = (@mode & ~O_ACCMODE ) | O_RDONLY
  end

  def force_write_only
    @mode = (@mode & ~O_ACCMODE) | O_WRONLY
  end

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

  def write(buf)
    buf_size = buf.size
    left = buf_size

    # FIXME: incomplete
  end

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

  def read(length, output_string=nil)
    length ||= IO.pagesize

    while true
      ensure_open

      storage = FFI::MemoryPointer.new(length)
      bytes_read = read_into_storage(length, storage)

      if bytes_read == -1
        if Errno.eql?(Errno::EAGAIN) || Errno.eql?(Errno::EINTR)
          redo
        else
          Errno.handle "read(2) failed"
        end

        return nil
      elsif bytes_read == 0
        @eof = true if length > 0
        return nil
      else
        break
      end
    end

    if output_string
      output_string.replace(storage.read_string(bytes_read))
    else
      output_string = storage.read_string(bytes_read)
    end
    
    @offset += bytes_read
    @eof = true if @offset == @total_size

    return output_string
  end

  def read_into_storage(count, storage)
    while true
      bytes_read = FFI::Platform::POSIX.read(descriptor, storage, count)

      if bytes_read == -1
        errno = Errno.errno

        if errno == Errno::EAGAIN || errno == Errno::EINTR
          ensure_open
          next
        else
          Errno.handle "read(2) failed"
        end
      else
        break
      end
    end

    return bytes_read
  end
  private :read_into_storage

  #  def read_primitive(number_of_bytes)
  #    Rubinius.primitive :io_sysread
  #    raise PrimitiveFailure, "IO::sysread primitive failed"
  #  end

  def write(str)
    Rubinius.primitive :io_write
    raise PrimitiveFailure, "IO#write primitive failed"
  end

  #  def write(str)
  #    FFI::Platform::POSIX.write(descriptor, str, str.size)
  #  end

  def read_if_available(size)
    Rubinius.primitive :io_read_if_available
    raise PrimitiveFailure, "IO#read_if_available primitive failed"
  end

  def raw_write(str)
    Rubinius.primitive :io_write_nonblock
    raise PrimitiveFailure, "IO#write_nonblock primitive failed"
  end

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

  def prim_seek(amount, whence)
    Rubinius.primitive :io_seek
    raise RangeError, "#{amount} is too big"
  end

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
