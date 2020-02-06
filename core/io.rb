class IO
  include Enumerable

  def self.fnmatch(pattern, path, flags)
    Rubinius.primitive :io_fnmatch
    raise PrimitiveFailure, "IO#fnmatch primitive failed"
  end

  def socket_recv(bytes, flags, type)
    Rubinius.primitive :io_socket_read
    raise PrimitiveFailure, "io_socket_read failed"
  end

  module TransferIO
    def send_io(io)
      Rubinius.primitive :io_send_io
      raise PrimitiveFailure, "IO#send_io failed"
    end

    def recv_fd
      Rubinius.primitive :io_recv_fd
      raise PrimitiveFailure, "IO#recv_fd failed"
    end
  end

  module WaitReadable; end
  module WaitWritable; end

  class EAGAINWaitReadable < Errno::EAGAIN
    include ::IO::WaitReadable
  end

  class EAGAINWaitWritable < Errno::EAGAIN
    include ::IO::WaitWritable
  end

  class EWOULDBLOCKWaitReadable < Errno::EAGAIN
    include WaitReadable
  end

  class EWOULDBLOCKWaitWritable < Errno::EAGAIN
    include WaitWritable
  end

  class EINPROGRESSWaitReadable < Errno::EINPROGRESS
    include WaitReadable
  end

  class EINPROGRESSWaitWritable < Errno::EINPROGRESS
    include WaitWritable
  end

  class FileDescriptor
    class RIOStream
      def self.close(io, allow_exception)
        Rubinius.primitive :rio_close
        raise PrimitiveFailure, "IO::FileDescriptor::RIOStream.close primitive failed"
      end
    end

    attr_reader :offset

    def self.choose_type(fd, io)
      stat = File::Stat.fstat(fd)

      case stat.ftype
      when "file"
        BufferedFileDescriptor.new(fd, stat, io)
      when "fifo", "characterSpecial"
        FIFOFileDescriptor.new(fd, stat, io)
      when "socket"
        SocketFileDescriptor.new(fd, stat, io)
      when "directory"
        DirectoryFileDescriptor.new(fd, stat, io)
      when "blockSpecial"
        raise "cannot make block special"
      when "link"
        raise "cannot make link"
      else
        BufferedFileDescriptor.new(fd, stat, io)
      end
    end

    def self.open_with_mode(path, mode, perm)
      fd = open_with_cloexec(path, mode, perm)

      if fd < 0
        Errno.handle("failed to open file")
      end

      return fd
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
        flags = FFI::Platform::POSIX.fcntl(new_fd, F_GETFD, 0)
        Errno.handle("fcntl(2) failed") if FFI.call_failed?(flags)
        flags = FFI::Platform::POSIX.fcntl(new_fd, F_SETFD, flags | FD_CLOEXEC)
        Errno.handle("fcntl(2) failed") if FFI.call_failed?(flags)
      end

      update_max_fd(new_fd)
    end

    def self.pagesize
      @pagesize ||= FFI::Platform::POSIX.getpagesize
    end

    def self.truncate(name, offset)
      raise RangeError, "bignum too big to convert into `long'" if offset.kind_of?(Bignum)

      status = FFI::Platform::POSIX.truncate(name, offset)
      Errno.handle("truncate(2) failed") if FFI.call_failed?(status)
      return status
    end

    def self.update_max_fd(new_fd)
      @@max_descriptors.get_and_set(new_fd)
    end

    def self.max_fd
      @@max_descriptors.get
    end

    def self.get_flags(fd)
      if IO::F_GETFL
        if FFI.call_failed?(flags = FFI::Platform::POSIX.fcntl(fd, IO::F_GETFL, 0))
          Errno.handle("fcntl(2) failed")
        end
      else
        flags = 0
      end
      flags
    end

    def self.clear_flag(flag, fd)
      flags = get_flags(fd)
      if (flags & flag) == 0
        flags &= ~flag
        if FFI.call_failed?(flags = FFI::Platform::POSIX.fcntl(fd, IO::F_SETFL, flags))
          Errno.handle("fcntl(2) failed")
        end
      end
    end

    def self.set_flag(flag, fd)
      flags = get_flags(fd)
      if (flags & flag) == 0
        flags |= flag
        if FFI.call_failed?(flags = FFI::Platform::POSIX.fcntl(fd, IO::F_SETFL, flags))
          Errno.handle("fcntl(2) failed")
        end
      end
    end

    def self.get_empty_8bit_buffer
      "".force_encoding(Encoding::ASCII_8BIT)
    end

    def initialize(fd, stat, io)
      @descriptor, @stat, @io = fd, stat, io
      acc_mode = FileDescriptor.get_flags(@descriptor)

      if acc_mode < 0
        # Assume it's closed.
        if Errno.eql?(Errno::EBADF::Errno)
          @descriptor = -1
        end

        @mode = nil
      else
        @mode = acc_mode
      end

      @sync = true
      @autoclose = false
      reset_positioning(@stat)

      # Don't bother to add finalization for stdio
      if @descriptor >= 3
        # Sometimes a FileDescriptor class is replaced (see IO#reopen) so we need to be
        # careful we don't finalize that descriptor. Probably need a way to cancel
        # the finalization when we are transferring an FD from one instance to another.
        ObjectSpace.define_finalizer(self)
      end
    end

    private :initialize

    def autoclose=(bool)
      @autoclose = bool
    end

    def descriptor
      @descriptor
    end

    def descriptor=(value)
      @descriptor = value
    end

    def mode
      @mode
    end

    def mode=(value)
      @mode = value
    end

    def sync
      @sync
    end

    def sync=(value)
      @sync = value
    end

    CLONG_OVERFLOW = 1 << 64

    def lseek(offset, whence=SEEK_SET)
      ensure_open

      # FIXME: check +amount+ to make sure it isn't too large
      raise RangeError if offset > CLONG_OVERFLOW

      position = FFI::Platform::POSIX.lseek(descriptor, offset, whence)

      if FFI.call_failed?(position)
        Errno.handle("seek failed: fd: #{descriptor}, offset: #{offset}, from: #{whence}")
      end

      @offset = position
      determine_eof

      return position
    end

    def read(length, output_string=nil)
      length ||= FileDescriptor.pagesize

      while true
        ensure_open

        storage = FFI::MemoryPointer.new(length)
        raise IOError, "read(2) failed to malloc a buffer for read length #{length}" if storage.null?
        bytes_read = read_into_storage(length, storage)

        if bytes_read == 0
          @eof = true if length > 0
          return nil
        else
          break
        end
      end

      if output_string
        output_string.replace(storage.read_string(bytes_read))
      else
        output_string = storage.read_string(bytes_read).force_encoding(Encoding::ASCII_8BIT)
      end

      @offset += bytes_read
      determine_eof

      return output_string
    end

    def read_into_storage(count, storage)
      while true
        bytes_read = FFI::Platform::POSIX.read(descriptor, storage, count)

        if FFI.call_failed?(bytes_read)
          errno = Errno.errno

          if errno == Errno::EAGAIN::Errno || errno == Errno::EINTR::Errno
            ensure_open
            next
          else
            Errno.handle "read(2) failed"
          end
        else
          break
        end
      end

      # before returning verify file hasn't been closed in another thread
      ensure_open

      return bytes_read
    end
    private :read_into_storage

    def write(str)
      buf_size = str.bytesize
      left = buf_size

      buffer = FFI::MemoryPointer.new(left)
      buffer.write_string(str)
      error = false

      while left > 0
        bytes_written = FFI::Platform::POSIX.write(@descriptor, buffer, left)

        if FFI.call_failed?(bytes_written)
          errno = Errno.errno
          if errno == Errno::EINTR::Errno || errno == Errno::EAGAIN::Errno
            # do a #select and wait for descriptor to become writable
            if blocking?
              Select.wait_for_writable(@descriptor)
              next
            else
              break
            end
          end

          error = true
          break
        end

        break if error

        left -= bytes_written
        buffer += bytes_written
        @offset += bytes_written

        if @offset > @total_size
          @total_size = @offset
          @eof = false # only a failed read can set EOF!
        end
      end

      Errno.handle("write failed") if error

      return(buf_size - left)
    end

    def close
      ensure_open
      fd = @descriptor

      if fd != -1
        # return early if this handle was promoted to a stream by a C-ext
        return if RIOStream.close(@io, true)
        ret_code = FFI::Platform::POSIX.close(fd)

        if FFI.call_failed?(ret_code)
          Errno.handle("close failed")
        elsif ret_code == 0
          # no op
        else
          raise IOError, "::close(): Unknown error on fd #{fd}"
        end
      end

      @descriptor = -1

      return nil
    end

    def determine_eof
      if @offset >= @total_size
        @eof = true
        @total_size += (@total_size - @offset)
        @total_size = @offset if @offset > @total_size
      else
        @eof = false
      end
    end
    private :determine_eof

    def eof?
      @eof
    end

    # This is NOT the same as close().
    def shutdown(how)
      ensure_open
      fd = descriptor

      if how != IO::SHUT_RD && how != IO::SHUT_WR && how != IO::SHUT_RDWR
        raise ArgumentError, "::shutdown(): Invalid `how` #{how} for fd #{fd}"
      end

      ret_code = FFI::Platform::POSIX.shutdown(fd, how)

      if FFI.call_failed?(ret_code)
        Errno.handle("shutdown(2) failed")
      elsif ret_code == 0
        if how == IO::SHUT_RDWR
          close
          self.descriptor = -2
        end
      else
        Errno.handle("::shutdown(): Unknown error on fd #{fd}")
      end

      return how
    end

    def ensure_open(fd=nil)
      fd ||= descriptor

      if fd.nil?
        raise IOError, "uninitialized stream"
      elsif fd == -1
        raise IOError, "closed stream"
      elsif fd == -2
        raise IOError, "shutdown stream"
      end
      return nil
    end

    def force_read_only
      @mode = (@mode & ~IO::O_ACCMODE ) | O_RDONLY
    end

    def force_write_only
      @mode = (@mode & ~IO::O_ACCMODE) | O_WRONLY
    end

    def read_only?
      (@mode & O_ACCMODE) == O_RDONLY
    end

    def write_only?
      (@mode & O_ACCMODE) == O_WRONLY
    end

    def read_write?
      (@mode & O_ACCMODE) == O_RDWR
    end

    def reopen(other_fd)
      current_fd = @descriptor

      if FFI.call_failed?(FFI::Platform::POSIX.dup2(other_fd, current_fd))
        Errno.handle("reopen")
      end

      set_mode
      reset_positioning

      return true
    end

    def reopen_path(path, mode)
      current_fd = @descriptor
      other_fd = FileDescriptor.open_with_cloexec(path, mode, 0666)

      Errno.handle("could not reopen path \"#{path}\"") if other_fd < 0

      if FFI.call_failed?(FFI::Platform::POSIX.dup2(other_fd, current_fd))
        if Errno.eql?(Errno::EBADF::Errno)
          # means current_fd is closed, so set ourselves to use the new fd and continue
          @descriptor = other_fd
        else
          FFI::Platform::POSIX.close(other_fd) if other_fd > 0
          Errno.handle("could not reopen path \"#{path}\"")
        end
      else
        FFI::Platform::POSIX.close(other_fd)
      end

      set_mode
      reset_positioning

      return true
    end

    def reset_positioning(stat=nil)
      # Discover final size of file so we can set EOF properly
      stat = File::Stat.fstat(@descriptor) unless stat
      @total_size = stat.size

      # We may have reopened a file descriptor that went from "file" to a different
      # ftype which doesn't allow seeking, so catch it here.
      if stat.ftype == "file"
        seek_positioning
      else
        @offset = 0
      end

      determine_eof
    end

    def seek_positioning
      @offset = lseek(0, SEEK_CUR) # find current position if we are reopening!
    end
    private :seek_positioning

    def set_mode
      @mode = FileDescriptor.get_flags(@descriptor)
    end

    def blocking?
      (FileDescriptor.get_flags(@descriptor) & O_NONBLOCK) == 0
    end

    def clear_nonblock
      flags = FileDescriptor.get_flags(@descriptor)
      FileDescriptor.clear_flag(O_NONBLOCK, @descriptor)
    end

    def set_nonblock
      flags = FileDescriptor.get_flags(@descriptor)
      FileDescriptor.set_flag(O_NONBLOCK, @descriptor)
    end

    def ftruncate(offset)
      ensure_open

      # FIXME: fail if +offset+ is too large, see C++ code

      status = FFI::Platform::POSIX.ftruncate(descriptor, offset)
      Errno.handle("ftruncate(2) failed") if FFI.call_failed?(status)
      return status
    end

    ##
    # Returns true if ios is associated with a terminal device (tty), false otherwise.
    #
    #  File.new("testfile").isatty   #=> false
    #  File.new("/dev/tty").isatty   #=> true
    def tty?
      ensure_open
      FFI::Platform::POSIX.isatty(@descriptor) == 1
    end

    def ttyname
      # Need to protect this call with a lock since the OS copies this string information
      # to an internal static object and returns a pointer to that object. Future calls
      # to #ttyname modify that same object. Therefore, we need to protect this from
      # race conditions.
      Rubinius.lock(self)
      name = FFI::Platform::POSIX.ttyname(descriptor)
      if name
        name
      else
        Errno.handle("ttyname(3) failed")
        nil
      end
    ensure
      Rubinius.unlock(self)
    end

    def inspect
      stat = File::Stat.fstat(@descriptor)
      "fd [#{descriptor}], mode [#{@mode}], total_size [#{@total_size}], offset [#{@offset}], eof [#{@eof}], stat.size [#{stat.size}], written? [#{@written}]"
    end

    def cancel_finalizer
      ObjectSpace.undefine_finalizer(self)
    end

    def __finalize__
      return if @descriptor.nil? || @descriptor == -1

      fd = @descriptor

      # Should flush a write buffer here if one exists. Current
      # implementation does not buffer writes internally, so this
      # is a no-op.

      # Do not close stdin/out/err (0, 1, and 2)
      if @descriptor > 3
        @descriptor = -1

        # Take care of any IO cleanup for the C API here. An IO may
        # have been promoted to a low-level RIO struct using #fdopen,
        # so we MUST use #fclose to clost it.
        return if RIOStream.close(@io, false)

        # Ignore any return code... don't care if it fails
        FFI::Platform::POSIX.close(fd) if @autoclose
      end
    end
  end # class FileDescriptor

  class BufferedFileDescriptor < FileDescriptor

    def buffer_reset
      @unget_buffer.clear
    end
    private :buffer_reset

    def read(length, output_string=nil)
      length ||= FileDescriptor.pagesize

      # Preferentially read from the buffer and then from the underlying
      # FileDescriptor.
      # FIXME: make this logic clearer.
      if length > @unget_buffer.size
        @offset += @unget_buffer.size
        length -= @unget_buffer.size

        str = @unget_buffer.inject(self.class.get_empty_8bit_buffer) { |sum, val| val.chr + sum }
        str2 = super(length, output_string)

        if str.size == 0 && str2.nil?
          determine_eof
          return nil
        elsif str2
          str += str2
        end
        buffer_reset
      elsif length == @unget_buffer.size
        @offset += length
        length -= @unget_buffer.size

        str = @unget_buffer.inject(self.class.get_empty_8bit_buffer) { |sum, val| val.chr + sum }
        buffer_reset
      else
        @offset += @unget_buffer.size
        str = self.class.get_empty_8bit_buffer

        length.times do
          str << @unget_buffer.pop
        end
      end

      if output_string
        output_string.replace(str)
      else
        output_string = str.force_encoding(Encoding::ASCII_8BIT)
      end

      determine_eof
      return output_string
    end

    def read_only_buffer(length)
      unless @unget_buffer.empty?
        if length >= @unget_buffer.size
          @offset += @unget_buffer.size
          length -= @unget_buffer.size

          str = @unget_buffer.inject(self.class.get_empty_8bit_buffer) { |sum, val| val.chr + sum }
          buffer_reset
          [str, length]
        else
          @offset += @unget_buffer.size
          str = self.class.get_empty_8bit_buffer

          length.times do
            str << @unget_buffer.pop
          end
          [str, 0]
        end
      else
        [nil, length]
      end
    end

    def seek(bytes, whence)
      # @offset may not match actual file pointer if there were calls to #unget.
      if whence == SEEK_CUR
        # adjust the number of bytes to seek based on how far ahead we are with the buffer
        bytes -= @unget_buffer.size
      end

      buffer_reset
      lseek(bytes, whence)
    end

    def sysread(byte_count)
      raise_if_buffering
      read(byte_count)
    end

    def sysseek(bytes, whence)
      raise_if_buffering
      lseek(bytes, whence)
    end

    def eof?
      super && @unget_buffer.empty?
    end

    def flush
      #@unget_buffer.clear
    end

    def raise_if_buffering
      raise IOError unless @unget_buffer.empty?
    end

    def reset_positioning(*args)
      @unget_buffer = []
      super
    end

    def write_nonblock(str)
      buffer_reset
      set_nonblock

      buf_size = str.bytesize
      left = buf_size

      if left > 0
        buffer = FFI::MemoryPointer.new(left)
        buffer.write_string(str)

        if FFI.call_failed?(bytes_written = FFI::Platform::POSIX.write(@descriptor, buffer, left))
          clear_nonblock
          Errno.raise_waitwritable("write_nonblock")
        end

        left -= bytes_written
        buffer += bytes_written
        @offset += bytes_written
      end

      return(buf_size - left)
    end

    def unget(byte)
      @offset -= 1
      @unget_buffer << byte
    end
  end # class BufferedFileDescriptor

  class FIFOFileDescriptor < BufferedFileDescriptor

    def self.connect_pipe_fds
      fds = FFI::MemoryPointer.new(:int, 2)

      Errno.handle("creating pipe failed") if FFI.call_failed?(FFI::Platform::POSIX.pipe(fds))
      fd0, fd1 = fds.read_array_of_int(2)

      FileDescriptor.new_open_fd(fd0)
      FileDescriptor.new_open_fd(fd1)

      return [fd0, fd1]
    end

    def initialize(fd, stat, io, mode=nil)
      super(fd, stat, self)
      @mode = mode if mode
      @eof = false # force to false
    end

    private :initialize

    def determine_eof
      if @offset >= @total_size
        @eof = true

        # No seeking allowed on a pipe, so its size is always its offset
        @total_size = @offset
      end
    end

    def eof?
      # The only way to confirm we are EOF with a pipe is to try to read from
      # it. If we fail, then we are EOF. If we succeed, then unget the byte
      # so that EOF is false (i.e. more to read).
      str = read(1)
      unget(str) if str
      super
    end

    def seek_positioning
      # no seeking allowed for pipes
      @offset = 0
    end

    def sysseek(offset, whence=SEEK_SET)
      # lseek does not work with pipes.
      raise Errno::ESPIPE
    end
  end # class FIFOFileDescriptor

  class DirectoryFileDescriptor < BufferedFileDescriptor
  end # class DirectoryFileDescriptor

  class SocketFileDescriptor < FIFOFileDescriptor
    def initialize(fd, stat, io)
      super

      @mode &= O_ACCMODE
      @sync = true
    end

    private :initialize

    def force_read_write
      @mode &= ~(O_RDONLY | O_WRONLY)
      @mode |= O_RDWR
    end
  end # class SocketFileDescriptor

  # Encapsulates all of the logic necessary for handling #select.
  class Select
    #eval(Rubinius::Config['rbx.platform.timeval.class'])

    class FDSet
      def self.new
        Rubinius.primitive :fdset_allocate
        raise PrimitiveFailure, "FDSet.allocate failed"
      end

      def zero
        Rubinius.primitive :fdset_zero
        raise PrimitiveFailure, "FDSet.zero failed"
      end

      def set(descriptor)
        Rubinius.primitive :fdset_set
        raise PrimitiveFailure, "FDSet.set failed"
      end

      def set?(descriptor)
        Rubinius.primitive :fdset_is_set
        raise PrimitiveFailure, "FDSet.set? failed"
      end

      def to_set
        Rubinius.primitive :fdset_to_set
        raise PrimitiveFailure, "FDSet.to_set failed"
      end
    end

    def self.fd_set_from_array(array)
      highest = -1
      fd_set = FDSet.new
      fd_set.zero

      array.each do |io|
        io = io[1] if io.is_a?(Array)
        descriptor = io.descriptor

        if descriptor >= FD_SETSIZE
          raise IOError
        end

        if descriptor >= 0
          highest = descriptor > highest ? descriptor : highest
          fd_set.set(descriptor)
        end
      end

      return [fd_set, highest]
    end

    def self.collect_set_fds(array, fd_set)
      return [] unless fd_set
      array.map do |io|
        key, io = if io.is_a?(Array)
          [io[0], io[1]]
        else
          [io, io]
        end

        if fd_set.set?(io.descriptor) || io.descriptor < 0
          key
        else
          nil
        end
      end.compact
    end

    def self.timer_add(time1, time2, result)
      result[:tv_sec] = time1[:tv_sec] + time2[:tv_sec]
      result[:tv_usec] = time1[:tv_usec] + time2[:tv_usec]

      if result[:tv_usec] >= 1_000_000
        result[:tv_sec] += 1
        result[:tv_usec] -= 1_000_000
      end
    end

    def self.timer_sub(time1, time2, result)
      result[:tv_sec] = time1[:tv_sec] - time2[:tv_sec]
      result[:tv_usec] = time1[:tv_usec] - time2[:tv_usec]

      if result[:tv_usec] < 0
        result[:tv_sec] -= 1
        result[:tv_usec] += 1_000_000
      end
    end

    def self.make_timeval_timeout(timeout)
      if timeout
        limit = Rubinius::FFI::Platform::POSIX::TimeVal.new
        future = Rubinius::FFI::Platform::POSIX::TimeVal.new

        limit[:tv_sec] = (timeout / 1_000_000.0).to_i
        limit[:tv_usec] = (timeout % 1_000_000.0).to_i

        # Get current time to be used if select is interrupted and we have to recalculate the sleep time
        if FFI.call_failed?(FFI::Platform::POSIX.gettimeofday(future, nil))
          Errno.handle("gettimeofday(2) failed")
        end

        timer_add(future, limit, future)
      end

      [limit, future]
    end

    def self.reset_timeval_timeout(time_limit, future)
      now = Rubinius::FFI::Platform::POSIX::TimeVal.new

      if FFI.call_failed?(FFI::Platform::POSIX.gettimeofday(now, nil))
        Errno.handle("gettimeofday(2) failed")
      end

      timer_sub(future, now, time_limit)
    end

    def self.readable_events(read_fd)
      fd_set = FDSet.new
      fd_set.zero
      fd_set.set(read_fd)

      # sets fields to zero by default
      timer = Rubinius::FFI::Platform::POSIX::TimeVal.new

      FFI::Platform::POSIX.select(read_fd + 1, fd_set.to_set, nil, nil, timer)
    end

    def self.wait_for_writable(fd)
      fd_set = FDSet.new
      fd_set.zero
      fd_set.set(fd)

      FFI::Platform::POSIX.select(fd + 1, nil, fd_set.to_set, nil, nil)
    end

    def self.select(readables, writables, errorables, timeout)
      read_set, highest_read_fd = readables.nil? ? [nil, nil] : fd_set_from_array(readables)
      write_set, highest_write_fd = writables.nil? ? [nil, nil] : fd_set_from_array(writables)
      error_set, highest_err_fd = errorables.nil? ? [nil, nil] : fd_set_from_array(errorables)
      max_fd = [highest_read_fd, highest_write_fd, highest_err_fd].compact.max || -1

      time_limit, future = make_timeval_timeout(timeout)

      events = 0
      loop do
        events = FFI::Platform::POSIX.select(max_fd + 1,
        (read_set ? read_set.to_set : nil),
        (write_set ? write_set.to_set : nil),
        (error_set ? error_set.to_set : nil),
        time_limit)

        if FFI.call_failed?(events)

          if Errno::EAGAIN::Errno == Errno.errno || Errno::EINTR::Errno == Errno.errno
            # return nil if async_interruption?
            time_limit = reset_timeval_timeout(time_limit, future)
            continue
          end

          Errno.handle("select(2) failed")
        end

        break
      end

      return nil if events.zero?

      output_fds = []
      output_fds << collect_set_fds(readables, read_set)
      output_fds << collect_set_fds(writables, write_set)
      output_fds << collect_set_fds(errorables, error_set)
      return output_fds
    end

    def self.validate_and_convert_argument(objects)
      if objects
        raise TypeError, "Argument must be an Array" unless objects.respond_to?(:to_ary)
        objects =
          objects.to_ary.map do |obj|
          if obj.kind_of? IO
            raise IOError, "closed stream" if obj.closed?
            obj
          else
            raise TypeError unless obj.respond_to?(:to_io)
            io = obj.to_io
            raise TypeError unless io
            raise IOError, "closed stream" if io.closed?
            [obj, io]
          end
        end
      end

      objects
    end
  end # class Select

  attr_accessor :external
  attr_accessor :internal
  # intended to only be used by IO.setup to associate a new FileDescriptor object with instance of IO
  attr_accessor :fd

  def self.binread(file, length=nil, offset=0)
    raise ArgumentError, "Negative length #{length} given" if !length.nil? && length < 0

    File.open(file, "r", :encoding => "ascii-8bit:-") do |f|
      f.seek(offset)
      f.read(length)
    end
  end

  def self.binwrite(file, string, *args)
    offset, opts = args
    opts ||= {}
    if offset.is_a?(Hash)
      offset, opts = nil, offset
    end

    mode, binary, external, internal, autoclose, encoding_options = IO.normalize_options(nil, opts)
    unless mode
      mode = File::CREAT | File::RDWR | File::BINARY
      mode |= File::TRUNC unless offset
    end
    File.open(file, mode, encoding_options.merge(:encoding => (external || "ASCII-8BIT"))) do |f|
      f.seek(offset || 0)
      f.write(string)
    end
  end

  class StreamCopier
    def initialize(from, to, length, offset)
      @length = length
      @offset = offset

      @from_io, @from = to_io(from, "rb")
      @to_io, @to = to_io(to, "wb")

      @method = read_method @from
    end

    private :initialize

    def to_io(obj, mode)
      if obj.kind_of? IO
        flag = true
        io = obj
      else
        flag = false

        if obj.kind_of? String
          io = File.open obj, mode
        elsif obj.respond_to? :to_path
          path = Rubinius::Type.coerce_to obj, String, :to_path
          io = File.open path, mode
        else
          io = obj
        end
      end

      return flag, io
    end

    def read_method(obj)
      if obj.respond_to? :readpartial
        :readpartial
      else
        :read
      end
    end

    def run
      @from.ensure_open_and_readable if @from.kind_of? IO
      @to.ensure_open_and_writable if @to.kind_of? IO

      if @offset
        if @from_io && !@from.pipe?
          saved_pos = @from.pos
        else
          saved_pos = 0
        end

        @from.seek @offset, IO::SEEK_CUR
      end

      size = @length ? @length : 16384
      bytes = 0

      begin
        while data = @from.__send__(@method, size, "")
          @to.write data
          bytes += data.bytesize

          break if @length && bytes >= @length
        end
      rescue EOFError
        # done reading
      end

      @to.flush if @to.kind_of? IO
      return bytes
    ensure
      if @from_io
        @from.pos = saved_pos if @offset
      else
        @from.close if @from.kind_of? IO
      end

      @to.close if @to.kind_of? IO unless @to_io
    end
  end # class StreamCopier

  def self.copy_stream(from, to, max_length=nil, offset=nil)
    StreamCopier.new(from, to, max_length, offset).run
  end

  def self.foreach(name, separator=undefined, limit=undefined, options=undefined)
    return to_enum(:foreach, name, separator, limit, options) unless block_given?

    name = Rubinius::Type.coerce_to_path name

    case separator
    when Fixnum
      options = limit
      limit = separator
      separator = $/
    when undefined
      separator = $/
    when Hash
      options = separator
      separator = $/
    when nil
      # do nothing
    else
      separator = StringValue(separator)
    end

    case limit
    when Fixnum, nil
      # do nothing
    when undefined
      limit = nil
    when Hash
      if undefined.equal? options
        options = limit
        limit = nil
      else
        raise TypeError, "can't convert Hash into Integer"
      end
    else
      value = limit
      limit = Rubinius::Type.try_convert limit, Fixnum, :to_int

      unless limit
        options = Rubinius::Type.coerce_to value, Hash, :to_hash
      end
    end

    case options
    when Hash
      # do nothing
    when undefined, nil
      options = { }
    else
      options = Rubinius::Type.coerce_to options, Hash, :to_hash
    end

    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io
    else
      options[:mode] = "r" unless options.key? :mode
      io = File.open(name, options)
    end

    begin
      while line = io.gets(separator, limit)
        yield line
      end
    ensure
      $_ = nil
      io.close
    end

    return nil
  end

  def self.readlines(name, separator=undefined, limit=undefined, options=undefined)
    lines = []
    saved_line = $_
    foreach(name, separator, limit, options) { |l| lines << l }

    $_ = saved_line
    lines
  end

  def self.read_encode(io, str, encoding_options)
    internal = io.internal_encoding
    external = io.external_encoding || Encoding.default_external

    if external.equal? Encoding::ASCII_8BIT
      str.force_encoding external
    elsif internal and external
      ec = Encoding::Converter.new(external, internal, (encoding_options || {}))
      ec.convert str
    else
      str.force_encoding external
    end
  end

  def self.write(file, string, *args)
    if args.size > 2
      raise ArgumentError, "wrong number of arguments (#{args.size + 2} for 2..3)"
    end

    offset, opts = args
    opts ||= {}
    if offset.is_a?(Hash)
      offset, opts = nil, offset
    end

    mode, binary, external, internal, autoclose, encoding_options = IO.normalize_options(nil, opts)
    unless mode
      mode = File::CREAT | File::WRONLY
      mode |= File::TRUNC unless offset
    end

    open_args = opts[:open_args] || [mode, :encoding => (external || "ASCII-8BIT")]
    open_args[1].merge!(encoding_options) if open_args[1].kind_of?(Hash)
    File.open(file, *open_args) do |f|
      f.seek(offset) if offset
      f.write(string)
    end
  end

  def self.for_fd(fd, mode=undefined, options=undefined)
    new fd, mode, options
  end

  def self.read(name, length_or_options=undefined, offset=0, options=nil)
    offset = 0 if offset.nil?
    name = Rubinius::Type.coerce_to_path name
    mode = "r"

    if undefined.equal? length_or_options
      length = undefined
    elsif Rubinius::Type.object_kind_of? length_or_options, Hash
      length = undefined
      offset = 0
      options = length_or_options
    elsif length_or_options
      offset = Rubinius::Type.coerce_to(offset || 0, Fixnum, :to_int)
      raise Errno::EINVAL, "offset must not be negative" if offset < 0

      length = Rubinius::Type.coerce_to(length_or_options, Fixnum, :to_int)
      raise ArgumentError, "length must not be negative" if length < 0
    else
      length = undefined
    end

    if options
      mode = options.delete(:mode) || "r"
    end

    # Detect pipe mode
    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io # child process
    else
      io = File.new(name, mode, options)
    end

    str = nil
    begin
      io.seek(offset) unless offset == 0
      if undefined.equal?(length)
        str = io.read
      else
        str = io.read length
      end
    ensure
      io.close
    end

    return str
  end

  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, IO, :to_io
  end

  def self.normalize_options(mode, options)
    mode = nil if undefined.equal?(mode)
    autoclose = true
    encoding_options = {}

    if undefined.equal?(options)
      options = Rubinius::Type.try_convert(mode, Hash, :to_hash)
      mode = nil if options
    elsif !options.nil?
      options = Rubinius::Type.try_convert(options, Hash, :to_hash)
      raise ArgumentError, "wrong number of arguments (3 for 1..2)" unless options
    end

    if mode
      mode = (Rubinius::Type.try_convert(mode, Integer, :to_int) or
              Rubinius::Type.coerce_to(mode, String, :to_str))
    end

    if options
      if optmode = options[:mode]
        optmode = (Rubinius::Type.try_convert(optmode, Integer, :to_int) or
                   Rubinius::Type.coerce_to(optmode, String, :to_str))
      end

      if mode
        raise ArgumentError, "mode specified twice" if optmode
      else
        mode = optmode
      end

      autoclose = !!options[:autoclose] if options.key?(:autoclose)
    end

    if mode.kind_of?(String)
      mode, external, internal = mode.split(":")
      raise ArgumentError, "invalid access mode" unless mode

      binary = true  if mode[1] === ?b
      binary = false if mode[1] === ?t
    elsif mode
      binary = true  if (mode & BINARY) != 0
    end

    if options
      if options[:textmode] and options[:binmode]
        raise ArgumentError, "both textmode and binmode specified"
      end

      if binary.nil?
        binary = options[:binmode]
      elsif options.key?(:textmode) or options.key?(:binmode)
        raise ArgumentError, "text/binary mode specified twice"
      end

      if !external and !internal
        external = options[:external_encoding]
        internal = options[:internal_encoding]
      elsif options[:external_encoding] or options[:internal_encoding] or options[:encoding]
        raise ArgumentError, "encoding specified twice"
      end

      if !external and !internal
        encoding = options[:encoding]

        if encoding.kind_of? Encoding
          external = encoding
        elsif !encoding.nil?
          encoding = StringValue(encoding)
          external, internal = encoding.split(':')
        end
      end

      [
       :invalid, :undef, :replace, :newline, :universal_newline, :crlf_newline,
       :cr_newline, :xml
      ].each do |options_key|
        encoding_options[options_key] = options[options_key] if options.has_key?(options_key)
      end
    end

    [mode, binary, external, internal, autoclose, encoding_options]
  end

  def self.open(*args)
    io = new(*args)

    return io unless block_given?

    begin
      yield io
    ensure
      begin
        io.close unless io.closed?
      rescue StandardError
        # nothing, just swallow them.
      end
    end
  end

  def self.parse_mode(mode)
    return mode if Rubinius::Type.object_kind_of? mode, Integer

    mode = StringValue(mode)

    ret = 0

    case mode[0]
    when ?r
      ret |= RDONLY
    when ?w
      ret |= WRONLY | CREAT | TRUNC
    when ?a
      ret |= WRONLY | CREAT | APPEND
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end

    return ret if mode.length == 1

    case mode[1]
    when ?+
        ret &= ~(RDONLY | WRONLY)
      ret |= RDWR
    when ?b
      ret |= BINARY
    when ?t
      ret &= ~BINARY
    when ?:
        warn("encoding options not supported in 1.8")
      return ret
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end

    return ret if mode.length == 2

    case mode[2]
    when ?+
        ret &= ~(RDONLY | WRONLY)
      ret |= RDWR
    when ?b
      ret |= BINARY
    when ?t
      ret &= ~BINARY
    when ?:
        warn("encoding options not supported in 1.8")
      return ret
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end

    ret
  end

  def self.pipe(external=nil, internal=nil, options=nil)
    # The use of #allocate is to make sure we create an IO obj. Would be so much
    # cleaner to just do a PipeIO class as a subclass, but that would not be
    # backward compatible. <sigh>
    fd0, fd1 = FIFOFileDescriptor.connect_pipe_fds
    lhs = allocate
    lhs.send(:new_pipe, fd0, external, internal, options, FileDescriptor::O_RDONLY, true)
    rhs = allocate
    rhs.send(:new_pipe, fd1, nil, nil, nil, FileDescriptor::O_WRONLY)

    if block_given?
      begin
        yield lhs, rhs
      ensure
        lhs.close unless lhs.closed?
        rhs.close unless rhs.closed?
      end
    else
      [lhs, rhs]
    end
  end

  def self.popen(*args)
    if env = Rubinius::Type.try_convert(args.first, Hash, :to_hash)
      args.shift
    end

    if io_options = Rubinius::Type.try_convert(args.last, Hash, :to_hash)
      args.pop
    end

    if args.size > 2
      raise ArgumentError, "#{__method__}: given #{args.size}, expected 1..2"
    end

    cmd, mode = args
    mode ||= "r"

    if cmd.kind_of? Array
      if sub_env = Rubinius::Type.try_convert(cmd.first, Hash, :to_hash)
        env = sub_env unless env
        cmd.shift
      end

      if exec_options = Rubinius::Type.try_convert(cmd.last, Hash, :to_hash)
        cmd.pop
      end
    end

    mode, binary, external, internal, autoclose, encoding_options =
      IO.normalize_options(mode, io_options || {})
    mode_int = parse_mode mode

    readable = false
    writable = false

    if mode_int & IO::RDWR != 0
      readable = true
      writable = true
    elsif mode_int & IO::WRONLY != 0
      writable = true
    else # IO::RDONLY
      readable = true
    end

    pa_read, ch_write = pipe if readable
    ch_read, pa_write = pipe if writable

    # We only need the Bidirectional pipe if we're reading and writing.
    # If we're only doing one, we can just return the IO object for
    # the proper half.
    if readable and writable
      # Transmogrify pa_read into a BidirectionalPipe object,
      # and then tell it abou it's pid and pa_write

      Rubinius::Unsafe.set_class pa_read, IO::BidirectionalPipe

      pipe = pa_read
      pipe.set_pipe_info(pa_write)
    elsif readable
      pipe = pa_read
    elsif writable
      pipe = pa_write
    else
      raise ArgumentError, "IO is neither readable nor writable"
    end

    pipe.binmode if binary
    pipe.set_encoding(external || Encoding.default_external, internal)

    if cmd == "-"
      pid = Rubinius::Mirror::Process.fork

      if !pid
        # Child
        begin
          if readable
            pa_read.close
            STDOUT.reopen ch_write
          end

          if writable
            pa_write.close
            STDIN.reopen ch_read
          end

          if block_given?
            yield nil
            exit! 0
          else
            return nil
          end
        rescue => e
          exit! 0
        end
      end
    else
      options = {}
      options[:in] = ch_read.fileno if ch_read
      options[:out] = ch_write.fileno if ch_write

      if io_options
        io_options.delete_if do |key, _|
          [:mode, :external_encoding, :internal_encoding,
           :encoding, :textmode, :binmode, :autoclose
           ].include? key
        end

        options.merge! io_options
      end

      if exec_options
        options.merge! exec_options
      end

      pid = Rubinius::Mirror::Process.spawn(env || {}, *cmd, options)
    end

    pipe.pid = pid

    ch_write.close if readable
    ch_read.close  if writable

    return pipe unless block_given?

    begin
      yield pipe
    ensure
      pipe.close unless pipe.closed?
    end
  end

  #
  # +select+ examines the IO object Arrays that are passed in
  # as +readables+, +writables+, and +errorables+ to see if any
  # of their descriptors are ready for reading, are ready for
  # writing, or have an exceptions pending respectively. An IO
  # may appear in more than one of the sets. Any of the three
  # sets may be +nil+ if you are not interested in those events.
  #
  # If +timeout+ is not nil, it specifies the number of seconds
  # to wait for events (maximum.) The number may be fractional,
  # conceptually up to a microsecond resolution.
  #
  # A +timeout+ of 0 indicates that each descriptor should be
  # checked once only, effectively polling the sets.
  #
  # Leaving the +timeout+ to +nil+ causes +select+ to block
  # infinitely until an event transpires.
  #
  # If the timeout expires without events, +nil+ is returned.
  # Otherwise, an [readable, writable, errors] Array of Arrays
  # is returned, only, with the IO objects that have events.
  #
  # @compatibility  MRI 1.8 and 1.9 require the +readables+ Array,
  #                 Rubinius does not.
  #
  def self.select(readables=nil, writables=nil, errorables=nil, timeout=nil)
    if timeout
      unless Rubinius::Type.object_kind_of? timeout, Numeric
        raise TypeError, "Timeout must be numeric"
      end

      raise ArgumentError, 'timeout must be positive' if timeout < 0

      # Microseconds, rounded down
      timeout = Integer(timeout * 1_000_000)
    end

    readables = IO::Select.validate_and_convert_argument(readables)
    writables = IO::Select.validate_and_convert_argument(writables)
    errorables = IO::Select.validate_and_convert_argument(errorables)

    IO::Select.select(readables, writables, errorables, timeout)
  end

  ##
  # Opens the given path, returning the underlying file descriptor as a Fixnum.
  #  IO.sysopen("testfile")   #=> 3
  def self.sysopen(path, mode = nil, perm = nil)
    path = Rubinius::Type.coerce_to_path path
    mode = parse_mode(mode || "r")
    perm ||= 0666

    FileDescriptor.open_with_mode path, mode, perm
  end

  #
  # Internally associate +io+ with the given descriptor.
  #
  # The +mode+ will be checked and set as the current mode if
  # the underlying descriptor allows it.
  #
  # The +sync+ attribute will also be set.
  #
  def self.setup(io, fd, mode=nil, sync=false)
    cur_mode = FileDescriptor.get_flags(fd)
    Errno.handle if cur_mode < 0

    cur_mode &= ACCMODE

    if mode
      mode = parse_mode(mode)
      mode &= ACCMODE

      if (cur_mode == RDONLY or cur_mode == WRONLY) and mode != cur_mode
        raise Errno::EINVAL, "Invalid new mode for existing descriptor #{fd}"
      end
    end

    # Check the given +io+ for a valid fd instance first. If it has one, cancel
    # the existing finalizer since we are about to allocate a new fd instance.
    if fd_obj = io.instance_variable_get(:@fd)
      fd_obj.cancel_finalizer
    end

    fd_obj = FileDescriptor.choose_type(fd, io)
    io.instance_variable_set(:@fd, fd_obj)
    raise "FD could not be allocated for fd [#{fd}]" unless fd_obj
    raise "No descriptor set for fd [#{fd}]" unless fd_obj.descriptor

    io.mode       = mode || cur_mode
    io.sync       = !!sync
    io.lineno     = 0 if io.lineno.nil?

    # FIXME - re-enable this somehow. Right now this breaks kernel/delta/io.rb when it
    # redefines STDIN/STDOUT/STDERR from the IO.open call. The new IO code has already
    # loaded so we can no longer access the object that STDIN/STDOUT/STDERR points to
    # via Ruby code, so the following code blows up.
    #    if STDOUT.respond_to?(:fileno) and not STDOUT.closed?
    #      io.sync ||= STDOUT.fileno == fd
    #    end
    #
    #    if STDERR.respond_to?(:fileno) and not STDERR.closed?
    #      io.sync ||= STDERR.fileno == fd
    #    end
  end

  #
  # Create a new IO associated with the given fd.
  #
  def initialize(fd, mode=undefined, options=undefined)
    if block_given?
      warn 'IO::new() does not take block; use IO::open() instead'
    end

    mode, binary, external, internal, @autoclose, @encoding_options =
      IO.normalize_options(mode, options)

    fd = Rubinius::Type.coerce_to fd, Integer, :to_int
    autoclose = @autoclose
    IO.setup self, fd, mode

    binmode if binary
    set_encoding external, internal

    if @external && !external
      @external = nil
    end

    if @internal
      if Encoding.default_external == Encoding.default_internal or
        (@external || Encoding.default_external) == Encoding::ASCII_8BIT
        @internal = nil
      end
    elsif !mode_read_only?
      if Encoding.default_external != Encoding.default_internal
        @internal = Encoding.default_internal
      end
    end

    unless @external
      if @binmode
        @external = Encoding::ASCII_8BIT
      elsif @internal or Encoding.default_internal
        @external = Encoding.default_external
      end
    end

    @pipe = false # FIXME
  end

  private :initialize

  ##
  # Obtains a new duplicate descriptor for the current one.
  def initialize_copy(original_io) # :nodoc:
    # Make a complete copy of the +original_io+ object including
    # the mode, binmode, path, position, lineno, and a new FD.
    dest_io = self

    fd = FFI::Platform::POSIX.dup(original_io.descriptor)

    # The system makes a shallow copy of all ivars, so this copy has
    # the same @fd as the original. That shallow copy is really only
    # relevant for primitive values (Fixnum, String, etc) and not
    # our own objects. Instantiate a new @fd.
    @fd = FileDescriptor.choose_type(fd, dest_io)
    dest_io.mode = original_io.mode
    dest_io.sync = original_io.sync
    dest_io.binmode if original_io.binmode?
    dest_io.autoclose = original_io.autoclose

    dest_io
  end
  private :initialize_copy

  def new_pipe(fd, external, internal, options, mode, do_encoding=false)
    @fd = FIFOFileDescriptor.new(fd, nil, self, mode)
    @lineno = 0
    @pipe = true

    # Why do we only set encoding for the "left hand side" pipe? Why not both?
    if do_encoding
      set_encoding((external || Encoding.default_external), (internal || Encoding.default_internal), options)
    end
  end
  private :new_pipe

  def super_inspect
    "<IO:#{object_id}> \n#{@fd.inspect}"
  end

  #  alias_method :prim_write, :write
  #  alias_method :prim_close, :close
  #  alias_method :prim_read, :read

  def descriptor
    @fd.descriptor if @fd
  end

  def descriptor=(value)
    @fd.descriptor = value if @fd
  end

  def mode
    @fd.mode if @fd
  end

  def mode=(value)
    @fd.mode = value if @fd
  end

  def sync
    @fd.sync if @fd
  end

  def sync=(value)
    @fd.sync = value if @fd
  end

  def advise(advice, offset = 0, len = 0)
    raise IOError, "stream is closed" if closed?
    raise TypeError, "advice must be a Symbol" unless advice.kind_of?(Symbol)

    if offset.kind_of?(Bignum) || len.kind_of?(Bignum)
      raise RangeError, "bignum too big to convert into `long'"
    end

    unless [:normal, :sequential, :random, :noreuse, :dontneed, :willneed].include? advice
      raise NotImplementedError, advice.inspect
    end

    advice = case advice
    when :normal; POSIX_FADV_NORMAL
    when :sequential; POSIX_FADV_SEQUENTIAL
    when :random; POSIX_FADV_RANDOM
    when :willneed; POSIX_FADV_WILLNEED
    when :dontneed; POSIX_FADV_DONTNEED
    when :noreuse; POSIX_FADV_NOREUSE
    end

    offset = Rubinius::Type.coerce_to offset, Integer, :to_int
    len = Rubinius::Type.coerce_to len, Integer, :to_int

    begin
      if FFI.call_failed?(FFI::Platform::POSIX.posix_fadvise(descriptor, offset, len, advice))
        Errno.handle("posix_fadvise(2) failed")
      end
    rescue NotImplementedError
      # MRI thinks platforms that don't support #advise should silently fail.
      # See https://bugs.ruby-lang.org/issues/11806
      nil
    end

    nil
  end

  def autoclose
    @autoclose
  end

  def autoclose?
    @autoclose
  end

  def autoclose=(bool)
    @fd.autoclose = bool
    @autoclose = !!bool
  end

  def binmode
    ensure_open

    @binmode = true
    @external = Encoding::BINARY
    @internal = nil

    # HACK what to do?
    self
  end

  def binmode?
    !@binmode.nil?
  end

  def close_on_exec=(value)
    if value
      fcntl(F_SETFD, fcntl(F_GETFD) | FD_CLOEXEC)
    else
      fcntl(F_SETFD, fcntl(F_GETFD) & ~FD_CLOEXEC)
    end
    nil
  end

  def close_on_exec?
    (fcntl(F_GETFD) & FD_CLOEXEC) != 0
  end

  def <<(obj)
    write(obj.to_s)
    return self
  end

  ##
  # Closes the read end of a duplex I/O stream (i.e., one
  # that contains both a read and a write stream, such as
  # a pipe). Will raise an IOError if the stream is not duplexed.
  #
  #  f = IO.popen("/bin/sh","r+")
  #  f.close_read
  #  f.readlines
  # produces:
  #
  #  prog.rb:3:in `readlines': not opened for reading (IOError)
  #   from prog.rb:3
  def close_read
    return if invalid_descriptor?

    if mode_write_only? || mode_read_write?
      raise IOError, 'closing non-duplex IO for reading'
    end

    @fd.close unless closed?
  end

  ##
  # Closes the write end of a duplex I/O stream (i.e., one
  # that contains both a read and a write stream, such as
  # a pipe). Will raise an IOError if the stream is not duplexed.
  #
  #  f = IO.popen("/bin/sh","r+")
  #  f.close_write
  #  f.print "nowhere"
  # produces:
  #
  #  prog.rb:3:in `write': not opened for writing (IOError)
  #   from prog.rb:3:in `print'
  #   from prog.rb:3
  def close_write
    return if invalid_descriptor?

    if mode_read_only? || mode_read_write?
      raise IOError, 'closing non-duplex IO for writing'
    end

    @fd.close unless closed?
  end

  ##
  # Returns true if ios is completely closed (for duplex
  # streams, both reader and writer), false otherwise.
  #
  #  f = File.new("testfile")
  #  f.close         #=> nil
  #  f.closed?       #=> true
  #  f = IO.popen("/bin/sh","r+")
  #  f.close_write   #=> nil
  #  f.closed?       #=> false
  #  f.close_read    #=> nil
  #  f.closed?       #=> true
  def closed?
    invalid_descriptor?
  end

  def dup
    ensure_open
    super # calls #initialize_copy
  end

  # Argument matrix for IO#gets and IO#each:
  #
  #  separator / limit | nil | >= 0 | < 0
  # ===================+=====+======+=====
  #  String (nonempty) |  A  |  B   |  C
  #                    +-----+------+-----
  #  ""                |  D  |  E   |  F
  #                    +-----+------+-----
  #  nil               |  G  |  H   |  I
  #

  class EachReader
    READ_SIZE = 512 # bytes

    def initialize(io, separator, limit, encoding_options)
      @io = io
      @separator = separator ? separator.force_encoding("ASCII-8BIT") : separator
      @limit = limit
      @encoding_options = encoding_options
      @skip = nil
    end

    private :initialize

    def each(&block)
      if @separator
        if @separator.empty?
          @separator = "\n\n"
          @skip = "\n"
        end

        if @limit
          read_to_separator_with_limit(&block)
        else
          read_to_separator(&block)
        end
      else
        if @limit
          read_to_limit(&block)
        else
          read_all(&block)
        end
      end
    end

    def read_and_yield_count_chars(str, buffer, byte_count, &block)
      str << buffer.slice!(0, byte_count)

      if @limit
        # Always read to char boundary because the +limit+ may have cut a multi-byte
        # character in the middle. Returning such a string would have an invalid encoding.
        if buffer.size < PEEK_AHEAD_LIMIT
          # Use nonblocking read since existing +buffer+ might already have a valid encoding
          # and we don't want to block forever if no more data is coming
          result = @io.get_empty_8bit_buffer

          # save and restore encodings around the read operation so that we ensure ASCII_8BIT
          internal, external = @io.internal, @io.external
          @io.external = Encoding::ASCII_8BIT
          (@io.read_nonblock(PEEK_AHEAD_LIMIT, result, exception: false) || @io.get_empty_8bit_buffer)
          @io.internal, @io.external = internal, external

          buffer << result
        end
        str, bytes_read = read_to_char_boundary(@io, str, buffer)
      else
        # We are confident that our +str+ ends on a char boundary
        str = IO.read_encode(@io, str, @encoding_options)
      end

      str.taint
      $. = @io.increment_lineno
      skip_contiguous_chars(buffer)

      # Unused bytes/chars should be saved for the next read. Since the block that we yield to
      # may +return+ we don't want to drop the bytes that are stored in +buffer+. To save,
      # unget them so the next read will fetch them again. This might be expensive and could
      # potentially use a little tuning. Maybe use an +unread(bytes)+ method which just moves
      # a pointer around. Think about this for the mmap stuff.
      @io.ungetc(buffer)
      buffer.clear

      yield str
    end

    def read_and_yield_entire_string(str, &block)
      str = IO.read_encode(@io, str, @encoding_options)
      str.taint
      $. = @io.increment_lineno
      yield str
    end

    # method A, D
    def read_to_separator(&block)
      str = @io.get_empty_8bit_buffer
      buffer = @io.get_empty_8bit_buffer
      separator_size = @separator.bytesize

      begin
        if buffer.size == 0
          buffer = @io.read(READ_SIZE)
        end

        break unless buffer.size > 0

        if count = buffer.index(@separator)
          # #index returns a 0-based location but we want a length (so +1) and it should include
          # the pattern/separator which may be >1. therefore, add the separator size.
          count += separator_size

          read_and_yield_count_chars(str, buffer, count, &block)
          str = @io.get_empty_8bit_buffer
        else
          str << buffer
          buffer.clear
        end
      end until buffer.size == 0 && @io.eof?

      str << buffer

      unless str.empty?
        read_and_yield_entire_string(str, &block)
      end
    end

    # method B, E

    def read_to_separator_with_limit(&block)
      str = @io.get_empty_8bit_buffer
      buffer = @io.get_empty_8bit_buffer
      separator_size = @separator.bytesize

      #TODO: implement ignoring encoding with negative limit
      wanted = limit = @limit.abs

      begin
        if buffer.size == 0
          buffer = @io.read(READ_SIZE)
        end

        break unless buffer && buffer.size > 0

        if count = buffer.index(@separator)
          # #index returns a 0-based location but we want a length (so +1) and it should include
          # the pattern/separator which may be >1. therefore, add the separator size.
          count += separator_size
          count = count < wanted ? count : wanted
          read_and_yield_count_chars(str, buffer, count, &block)

          str = @io.get_empty_8bit_buffer
        else
          if wanted < buffer.size
            read_and_yield_count_chars(str, buffer, wanted, &block)
            str = @io.get_empty_8bit_buffer
          else
            str << buffer
            wanted -= buffer.size
            buffer.clear
          end
        end
      end until buffer.size == 0 && @io.eof?

      unless str.empty?
        read_and_yield_entire_string(str, &block)
      end
    end

    # Method G
    def read_all(&block)
      str = @io.get_empty_8bit_buffer

      begin
        str << @io.read
      end until @io.eof?

      unless str.empty?
        read_and_yield_entire_string(str, &block)
      end
    end

    # Method H
    # Only ever called if separator or $/ is forced to nil
    def read_to_limit(&block)
      str = @io.get_empty_8bit_buffer
      wanted = limit = @limit.abs

      begin
        str << @io.read(wanted)
        read_and_yield_count_chars(str, @io.get_empty_8bit_buffer, str.bytesize, &block)
        str = @io.get_empty_8bit_buffer
      end until @io.eof?

      unless str.empty?
        read_and_yield_entire_string(str, &block)
      end
    end

    # Utility methods

    def try_to_force_encoding(io, str)
      str.force_encoding(io.external_encoding || Encoding.default_external)

      IO.read_encode(io, str, @encoding_options)
    end

    PEEK_AHEAD_LIMIT = 16

    def read_to_char_boundary(io, str, buffer)
      str.force_encoding(io.external_encoding || Encoding.default_external)
      return [IO.read_encode(io, str, @encoding_options), 0] if str.valid_encoding?

      peek_ahead = 0
      while buffer.size > 0 and peek_ahead < PEEK_AHEAD_LIMIT
        str.force_encoding Encoding::ASCII_8BIT
        substring = buffer.slice!(0, 1)
        str << substring
        peek_ahead += 1

        str.force_encoding(io.external_encoding || Encoding.default_external)
        if str.valid_encoding?
          return [IO.read_encode(io, str, @encoding_options), peek_ahead]
        end
      end

      [IO.read_encode(io, str, @encoding_options), peek_ahead]
    end

    # Advances the buffer index past any number of contiguous
    # characters == +skip+ and throws away that data. For
    # example, if +skip+ is ?\n and the buffer contents are
    # "\n\n\nAbc...", the buffer will discard all chars
    # up to 'A'.
    def skip_contiguous_chars(buffer)
      return 0 unless @skip

      skip_count = 0
      skip_count += 1 while buffer[skip_count] == @skip
      if skip_count > 0
        slice = buffer.slice!(0, skip_count)
        slice.bytesize
      else
        0
      end
    end
  end

  def increment_lineno
    @lineno += 1
  end

  ##
  # Return a string describing this IO object.
  def inspect
    if @fd
      if @fd.descriptor != -1
        "#<#{self.class}:fd #{@fd.descriptor}>"
      else
        "#<#{self.class}:(closed)>"
      end
    else
      "#<#{self.class}:fd nil>"
    end
  end

  def lines(*args, &block)
    if block_given?
      each_line(*args, &block)
    else
      to_enum :each_line, *args
    end
  end

  def each(sep_or_limit=$/, limit=nil, &block)
    return to_enum(:each, sep_or_limit, limit) unless block_given?

    ensure_open_and_readable

    if limit
      limit = Rubinius::Type.coerce_to limit, Integer, :to_int
      sep = sep_or_limit ? StringValue(sep_or_limit) : nil
    else
      case sep_or_limit
      when String
        sep = sep_or_limit
      when nil
        sep = nil
      else
        unless sep = Rubinius::Type.check_convert_type(sep_or_limit, String, :to_str)
          sep = $/
          limit = Rubinius::Type.coerce_to sep_or_limit, Integer, :to_int
        end
      end
    end

    raise ArgumentError, "limit of 0 is invalid" if limit && limit.zero?
    return if eof?

    EachReader.new(self, sep, limit, @encoding_options).each(&block)

    self
  end

  def each_line(sep_or_limit=$/, limit=nil, &block)
    if limit
      limit = Rubinius::Type.coerce_to limit, Integer, :to_int
      sep = sep_or_limit ? StringValue(sep_or_limit) : nil
      raise ArgumentError, "invalid limit: 0 for each_line" if limit.zero?
    else
      case sep_or_limit
      when String
        sep = sep_or_limit
      when nil
        sep = nil
      else
        unless sep = Rubinius::Type.check_convert_type(sep_or_limit, String, :to_str)
          sep = $/
          limit = Rubinius::Type.coerce_to sep_or_limit, Integer, :to_int
          raise ArgumentError, "invalid limit: 0 for each_line" if limit.zero?
          sep_or_limit = sep
        end
      end
    end

    each(sep_or_limit, limit, &block)
  end

  def each_byte
    return to_enum(:each_byte) unless block_given?

    yield getbyte until eof?

    self
  end

  alias_method :bytes, :each_byte

  def each_char
    return to_enum :each_char unless block_given?
    ensure_open_and_readable

    while char = getc
      yield char
    end

    self
  end

  alias_method :chars, :each_char

  def each_codepoint
    return to_enum :each_codepoint unless block_given?
    ensure_open_and_readable

    while char = getc
      yield char.ord
    end

    self
  end

  alias_method :codepoints, :each_codepoint


  ##
  # Set the pipe so it is at the end of the file
  def eof!
    @eof = true
  end

  ##
  # Returns true if ios is at end of file that means
  # there are no more data to read. The stream must be
  # opened for reading or an IOError will be raised.
  #
  #  f = File.new("testfile")
  #  dummy = f.readlines
  #  f.eof   #=> true
  # If ios is a stream such as pipe or socket, IO#eof?
  # blocks until the other end sends some data or closes it.
  #
  #  r, w = IO.pipe
  #  Thread.new { sleep 1; w.close }
  #  r.eof?  #=> true after 1 second blocking
  #
  #  r, w = IO.pipe
  #  Thread.new { sleep 1; w.puts "a" }
  #  r.eof?  #=> false after 1 second blocking
  #
  #  r, w = IO.pipe
  #  r.eof?  # blocks forever
  #
  # Note that IO#eof? reads data to a input buffer.
  # So IO#sysread doesn't work with IO#eof?.
  def eof?
    ensure_open_and_readable
    @fd.eof?
  end

  alias_method :eof, :eof?

  def ensure_open_and_readable
    ensure_open
    raise IOError, "not opened for reading" if mode_write_only?
  end

  def ensure_open_and_writable
    ensure_open
    raise IOError, "not opened for writing" if mode_read_only?
  end

  def external_encoding
    return @external if @external
    return Encoding.default_external if mode_read_only?
  end

  ##
  # Provides a mechanism for issuing low-level commands to
  # control or query file-oriented I/O streams. Arguments
  # and results are platform dependent. If arg is a number,
  # its value is passed directly. If it is a string, it is
  # interpreted as a binary sequence of bytes (Array#pack
  # might be a useful way to build this string). On Unix
  # platforms, see fcntl(2) for details. Not implemented on all platforms.
  def fcntl(command, arg=0)
    ensure_open

    if !arg
      arg = 0
    elsif arg == true
      arg = 1
    elsif arg.kind_of? String
      raise NotImplementedError, "cannot handle String"
    else
      arg = Rubinius::Type.coerce_to arg, Fixnum, :to_int
    end

    command = Rubinius::Type.coerce_to command, Fixnum, :to_int
    FFI::Platform::POSIX.fcntl descriptor, command, arg
  end

  def internal_encoding
    @internal
  end

  ##
  # Provides a mechanism for issuing low-level commands to
  # control or query file-oriented I/O streams. Arguments
  # and results are platform dependent. If arg is a number,
  # its value is passed directly. If it is a string, it is
  # interpreted as a binary sequence of bytes (Array#pack
  # might be a useful way to build this string). On Unix
  # platforms, see fcntl(2) for details. Not implemented on all platforms.
  def ioctl(command, arg=0)
    ensure_open

    if !arg
      real_arg = 0
    elsif arg == true
      real_arg = 1
    elsif arg.kind_of? String
      # This could be faster.
      buffer_size = arg.bytesize
      # On BSD and Linux, we could read the buffer size out of the ioctl value.
      # Most Linux ioctl codes predate the convention, so a fallback like this
      # is still necessary.
      buffer_size = 4096 if buffer_size < 4096
      buffer = FFI::MemoryPointer.new buffer_size
      buffer.write_string arg, arg.bytesize
      real_arg = buffer.address
    else
      real_arg = Rubinius::Type.coerce_to arg, Fixnum, :to_int
    end

    command = Rubinius::Type.coerce_to command, Fixnum, :to_int
    ret = FFI::Platform::POSIX.ioctl descriptor, command, real_arg
    Errno.handle if ret < 0
    if arg.kind_of?(String)
      arg.replace buffer.read_string(buffer_size)
      buffer.free
    end
    ret
  end

  ##
  # Returns an integer representing the numeric file descriptor for ios.
  #
  #  $stdin.fileno    #=> 0
  #  $stdout.fileno   #=> 1
  def fileno
    ensure_open
    return @fd.descriptor
  end

  alias_method :to_i, :fileno

  ##
  # Flushes any buffered data within ios to the underlying
  # operating system (note that this is Ruby internal
  # buffering only; the OS may buffer the data as well).
  #
  #  $stdout.print "no newline"
  #  $stdout.flush
  # produces:
  #
  #  no newline
  def flush
    ensure_open
    #@fd.reset_positioning
    return self
  end

  def force_read_only
    @fd.force_read_only
  end

  def force_write_only
    @fd.force_write_only
  end

  ##
  # Immediately writes all buffered data in ios to disk. Returns
  # nil if the underlying operating system does not support fsync(2).
  # Note that fsync differs from using IO#sync=. The latter ensures
  # that data is flushed from Ruby's buffers, but does not guarantee
  # that the underlying operating system actually writes it to disk.
  def fsync
    flush

    err = FFI::Platform::POSIX.fsync descriptor

    Errno.handle 'fsync(2)' if err < 0

    return err
  end

  def getbyte
    ensure_open
    byte = read(1)
    return(byte ? byte.ord : nil)
  end

  ##
  # Gets the next 8-bit byte (0..255) from ios.
  # Returns nil if called at end of file.
  #
  #  f = File.new("testfile")
  #  f.getc   #=> 84
  #  f.getc   #=> 104
  def getc
    ensure_open
    return if eof?

    char = get_empty_8bit_buffer
    begin
      char.force_encoding Encoding::ASCII_8BIT
      char << read(1)

      char.force_encoding(self.external_encoding || Encoding.default_external)
      if char.chr_at(0)
        return IO.read_encode(self, char, @encoding_options)
      end
    end until eof?

    return nil
  end

  def gets(sep_or_limit=$/, limit=nil)
    each sep_or_limit, limit do |line|
      $_ = line if line
      return line
    end

    return nil
  end

  ##
  # Returns the current line number in ios. The
  # stream must be opened for reading. lineno
  # counts the number of times gets is called,
  # rather than the number of newlines encountered.
  # The two values will differ if gets is called with
  # a separator other than newline. See also the $. variable.
  #
  #  f = File.new("testfile")
  #  f.lineno   #=> 0
  #  f.gets     #=> "This is line one\n"
  #  f.lineno   #=> 1
  #  f.gets     #=> "This is line two\n"
  #  f.lineno   #=> 2
  def lineno
    ensure_open

    return @lineno
  end

  ##
  # Manually sets the current line number to the
  # given value. $. is updated only on the next read.
  #
  #  f = File.new("testfile")
  #  f.gets                     #=> "This is line one\n"
  #  $.                         #=> 1
  #  f.lineno = 1000
  #  f.lineno                   #=> 1000
  #  $. # lineno of last read   #=> 1
  #  f.gets                     #=> "This is line two\n"
  #  $. # lineno of last read   #=> 1001
  def lineno=(line_number)
    ensure_open

    raise TypeError if line_number.nil?

    @lineno = Integer(line_number)
  end

  def mode_read_only?
    @fd.read_only?
  end
  private :mode_read_only?

  def mode_read_write?
   @fd.read_write?
  end
  private :mode_read_write?

  def mode_write_only?
    @fd.write_only?
  end
  private :mode_write_only?

  ##
  # FIXME
  # Returns the process ID of a child process
  # associated with ios. This will be set by IO::popen.
  #
  #  pipe = IO.popen("-")
  #  if pipe
  #    $stderr.puts "In parent, child pid is #{pipe.pid}"
  #  else
  #    $stderr.puts "In child, pid is #{$$}"
  #  end
  # produces:
  #
  #  In child, pid is 26209
  #  In parent, child pid is 26209
  def pid
    raise IOError, 'closed stream' if closed?
    @pid
  end

  attr_writer :pid

  def pipe=(v)
    @pipe = !!v
  end

  def pipe?
    @pipe
  end

  ##
  #
  def pos
    ensure_open
    @fd.offset
  end

  alias_method :tell, :pos

  ##
  # Seeks to the given position (in bytes) in ios.
  #
  #  f = File.new("testfile")
  #  f.pos = 17
  #  f.gets   #=> "This is line two\n"
  def pos=(offset)
    seek offset, SEEK_SET
  end

  ##
  # Writes each given argument.to_s to the stream or $_ (the result of last
  # IO#gets) if called without arguments. Appends $\.to_s to output. Returns
  # nil.
  def print(*args)
    if args.empty?
      write $_.to_s
    else
      args.each { |o| write o.to_s }
    end

    write $\.to_s
    nil
  end

  ##
  # Formats and writes to ios, converting parameters under
  # control of the format string. See Kernel#sprintf for details.
  def printf(fmt, *args)
    fmt = StringValue(fmt)
    write ::Rubinius::Sprinter.get(fmt).call(*args)
  end

  ##
  # If obj is Numeric, write the character whose code is obj,
  # otherwise write the first character of the string
  # representation of obj to ios.
  #
  #  $stdout.putc "A"
  #  $stdout.putc 65
  # produces:
  #
  #  AA
  def putc(obj)
    if Rubinius::Type.object_kind_of? obj, String
      write obj.substring(0, 1)
    else
      byte = Rubinius::Type.coerce_to(obj, Integer, :to_int) & 0xff
      write byte.chr
    end

    return obj
  end

  ##
  # Writes the given objects to ios as with IO#print.
  # Writes a record separator (typically a newline)
  # after any that do not already end with a newline
  # sequence. If called with an array argument, writes
  # each element on a new line. If called without arguments,
  # outputs a single record separator.
  #
  #  $stdout.puts("this", "is", "a", "test")
  # produces:
  #
  #  this
  #  is
  #  a
  #  test
  def puts(*args)
    if args.empty?
      write DEFAULT_RECORD_SEPARATOR
    else
      args.each do |arg|
        if arg.equal? nil
          str = ""
        elsif arg.kind_of? String
          str = arg
        elsif Thread.guarding? arg
          str = "[...]"
        else
          Thread.recursion_guard arg do
            begin
              arg.to_ary.each { |a| puts a }
            rescue NoMethodError
              unless (str = arg.to_s).kind_of? String
                str = "#<#{arg.class}:0x#{arg.object_id.to_s(16)}>"
              end
            end
          end
        end

        if str
          write str
          write DEFAULT_RECORD_SEPARATOR unless str.suffix?(DEFAULT_RECORD_SEPARATOR)
        end
      end
    end

    nil
  end

  def read(length=nil, buffer=nil)
    ensure_open_and_readable
    buffer = StringValue(buffer) if buffer

    unless length
      str = IO.read_encode(self, read_all, @encoding_options)
      return str unless buffer

      return buffer.replace(str)
    end

    str = ""
    emulate_blocking_read do
      read_nonblock(length, str)
    end

    if str.empty? && length > 0
      str = nil
    end

    if str
      if buffer
        buffer.replace str.force_encoding(buffer.encoding)
      else
        str.force_encoding Encoding::ASCII_8BIT
      end
    else
      buffer.clear if buffer
      nil
    end
  end

  ##
  # Reads all input until +#eof?+ is true. Returns the input read.
  # If the buffer is already exhausted, returns +""+.
  def read_all
    str = ""
    begin
      buffer = ""
      emulate_blocking_read do
        read_nonblock(FileDescriptor.pagesize, buffer)
      end
      str << buffer
    end until eof?

    str
  end

  private :read_all

  def read_if_available(bytes)
    return "" if bytes.zero?
    buffer, bytes = @fd.read_only_buffer(bytes)

    events = IO::Select.readable_events(descriptor)
    if events == 0 && !buffer
      Errno.raise_waitreadable("no data ready")
      return ""
    elsif events < 0 && !buffer
      Errno.handle("read(2) failed")
      return ""
    elsif events == 0 && buffer
      # we were able to read from the buffer but no more data is waiting
      return buffer
    end

    # if we get here then we have data to read from the descriptor
    str = ""
    bytes_read = @fd.read(bytes, str)

    if bytes_read.nil?
      # there's a chance the read could fail even when we have data read from the buffer
      # to return to caller
      return (nil || buffer)
    else
      # combine what we read from the buffer with what we read from the descriptor
      buffer = buffer.to_s + str
      return buffer
    end
  end
  # defined in bootstrap, used here.
  private :read_if_available

  ##
  # Reads at most maxlen bytes from ios using read(2) system
  # call after O_NONBLOCK is set for the underlying file descriptor.
  #
  # If the optional outbuf argument is present, it must reference
  # a String, which will receive the data.
  #
  # read_nonblock just calls read(2). It causes all errors read(2)
  # causes: EAGAIN, EINTR, etc. The caller should care such errors.
  #
  # read_nonblock causes EOFError on EOF.
  #
  # If the read buffer is not empty, read_nonblock reads from the
  # buffer like readpartial. In this case, read(2) is not called.
  def read_nonblock(size, buffer=nil, exception: true)
    raise ArgumentError, "illegal read size" if size < 0
    ensure_open

    buffer = StringValue buffer if buffer

    unless @fd.blocking?
      @fd.set_nonblock
      nonblock_reset = true
    end

    begin
      str = read_if_available(size)
    rescue EAGAINWaitReadable => exc
      raise exc if exception

      return :wait_readable
    end

    if str
      if buffer
        buffer.replace(str)
        IO.read_encode(self, buffer, @encoding_options)
      else
        IO.read_encode(self, str, @encoding_options)
      end
    else
      raise EOFError, "stream closed" if exception
    end
  ensure
    @fd.clear_nonblock if nonblock_reset
  end

  ##
  # Reads a character as with IO#getc, but raises an EOFError on end of file.
  def readchar
    char = getc
    raise EOFError, 'end of file reached' unless char
    char
  end

  def readbyte
    byte = getbyte
    raise EOFError, "end of file reached" unless byte
    #raise EOFError, "end of file" unless bytes # bytes/each_byte is deprecated, FIXME - is this line necessary?
    byte
  end

  ##
  # Reads a line as with IO#gets, but raises an EOFError on end of file.
  def readline(sep=$/)
    out = gets(sep)
    raise EOFError, "end of file" unless out
    return out
  end

  ##
  # Reads all of the lines in ios, and returns them in an array.
  # Lines are separated by the optional sep_string. If sep_string
  # is nil, the rest of the stream is returned as a single record.
  # The stream must be opened for reading or an IOError will be raised.
  #
  #  f = File.new("testfile")
  #  f.readlines[0]   #=> "This is line one\n"
  def readlines(sep=$/)
    sep = StringValue sep if sep

    old_line = $_
    ary = Array.new
    while line = gets(sep)
      ary << line
    end
    $_ = old_line

    ary
  end

  ##
  # Reads at most maxlen bytes from the I/O stream. It blocks
  # only if ios has no data immediately available. It doesn‘t
  # block if some data available. If the optional outbuf argument
  # is present, it must reference a String, which will receive the
  # data. It raises EOFError on end of file.
  #
  # readpartial is designed for streams such as pipe, socket, tty,
  # etc. It blocks only when no data immediately available. This
  # means that it blocks only when following all conditions hold.
  #
  # the buffer in the IO object is empty.
  # the content of the stream is empty.
  # the stream is not reached to EOF.
  # When readpartial blocks, it waits data or EOF on the stream.
  # If some data is reached, readpartial returns with the data.
  # If EOF is reached, readpartial raises EOFError.
  #
  # When readpartial doesn‘t blocks, it returns or raises immediately.
  # If the buffer is not empty, it returns the data in the buffer.
  # Otherwise if the stream has some content, it returns the data in
  # the stream. Otherwise if the stream is reached to EOF, it raises EOFError.
  #
  #  r, w = IO.pipe           #               buffer          pipe content
  #  w << "abc"               #               ""              "abc".
  #  r.readpartial(4096)      #=> "abc"       ""              ""
  #  r.readpartial(4096)      # blocks because buffer and pipe is empty.
  #
  #  r, w = IO.pipe           #               buffer          pipe content
  #  w << "abc"               #               ""              "abc"
  #  w.close                  #               ""              "abc" EOF
  #  r.readpartial(4096)      #=> "abc"       ""              EOF
  #  r.readpartial(4096)      # raises EOFError
  #
  #  r, w = IO.pipe           #               buffer          pipe content
  #  w << "abc\ndef\n"        #               ""              "abc\ndef\n"
  #  r.gets                   #=> "abc\n"     "def\n"         ""
  #  w << "ghi\n"             #               "def\n"         "ghi\n"
  #  r.readpartial(4096)      #=> "def\n"     ""              "ghi\n"
  #  r.readpartial(4096)      #=> "ghi\n"     ""              ""
  # Note that readpartial behaves similar to sysread. The differences are:
  #
  # If the buffer is not empty, read from the buffer instead
  # of "sysread for buffered IO (IOError)".
  # It doesn‘t cause Errno::EAGAIN and Errno::EINTR. When readpartial
  # meets EAGAIN and EINTR by read system call, readpartial retry the system call.
  # The later means that readpartial is nonblocking-flag insensitive. It
  # blocks on the situation IO#sysread causes Errno::EAGAIN as if the fd is blocking mode.
  def readpartial(size, buffer=nil)
    raise ArgumentError, 'negative string size' unless size >= 0
    ensure_open

    if buffer
      buffer = StringValue(buffer)

      buffer.shorten! buffer.bytesize

      return buffer if size == 0

      data = nil
      begin
        data = read_nonblock(size)
      rescue IO::WaitReadable
        IO.select([self])
        retry
      rescue IO::WaitWritable
        IO.select(nil, [self])
        retry
      end

      buffer.replace(data)

      return buffer
    else
      return "" if size == 0

      data = nil
      begin
        data = read_nonblock(size)
      rescue IO::WaitReadable
        IO.select([self])
        retry
      rescue IO::WaitWritable
        IO.select(nil, [self])
        retry
      end

      return data
    end
  end

  ##
  # Reassociates ios with the I/O stream given in other_IO or to
  # a new stream opened on path. This may dynamically change the
  # actual class of this stream.
  #
  #  f1 = File.new("testfile")
  #  f2 = File.new("testfile")
  #  f2.readlines[0]   #=> "This is line one\n"
  #  f2.reopen(f1)     #=> #<File:testfile>
  #  f2.readlines[0]   #=> "This is line one\n"
  def reopen(other, mode=undefined)
    if other.respond_to?(:to_io)
      flush

      if other.kind_of? IO
        io = other
      else
        io = other.to_io
        unless io.kind_of? IO
          raise TypeError, "#to_io must return an instance of IO"
        end
      end

      # Note: this is the whole reason that FileDescriptor#ensure_open takes an argument.
      #
      ensure_open(io.descriptor)
      #      io.reset_buffering

      @fd.reopen(io.descriptor)

      # When reopening we may be going from a Pipe to a File or vice versa. Let the
      # system figure out the proper FD class.
      @fd.cancel_finalizer # cancel soon-to-be-overwritten instance's finalizer
      @fd = FileDescriptor.choose_type(descriptor, self)
      Rubinius::Unsafe.set_class self, io.class
      if io.respond_to?(:path)
        @path = io.path
      end

      seek(other.pos, SEEK_SET) rescue Errno::ESPIPE
    else
      flush unless closed?

      # If a mode isn't passed in, use the mode that the IO is already in.
      if undefined.equal? mode
        mode = @fd.mode
        # If this IO was already opened for writing, we should
        # create the target file if it doesn't already exist.
        if mode_read_write? || mode_write_only?
          mode |= CREAT
        end
      else
        mode = IO.parse_mode(mode)
      end

      reopen_path Rubinius::Type.coerce_to_path(other), mode

      unless closed?
        seek(0, SEEK_SET) rescue Errno::ESPIPE
      end
    end

    self
  end

  def reopen_path(path, mode)
    status =  @fd.reopen_path(path, mode)
    @fd.cancel_finalizer
    @fd = FileDescriptor.choose_type(descriptor, self)
    return status
  end

  ##
  # Internal method used to reset the state of the buffer, including the
  # physical position in the stream.
  def reset_buffering
    #    ##@ibuffer.unseek! self
  end

  ##
  # Positions ios to the beginning of input, resetting lineno to zero.
  #
  #  f = File.new("testfile")
  #  f.readline   #=> "This is line one\n"
  #  f.rewind     #=> 0
  #  f.lineno     #=> 0
  #  f.readline   #=> "This is line one\n"
  def rewind
    seek 0
    @lineno = 0
    return 0
  end

  ##
  # Seeks to a given offset +amount+ in the stream according to the value of whence:
  #
  # IO::SEEK_CUR  | Seeks to _amount_ plus current position
  # --------------+----------------------------------------------------
  # IO::SEEK_END  | Seeks to _amount_ plus end of stream (you probably
  #               | want a negative value for _amount_)
  # --------------+----------------------------------------------------
  # IO::SEEK_SET  | Seeks to the absolute location given by _amount_
  # Example:
  #
  #  f = File.new("testfile")
  #  f.seek(-13, IO::SEEK_END)   #=> 0
  #  f.readline                  #=> "And so on...\n"
  def seek(amount, whence=SEEK_SET)
    flush

    @eof = false

    @fd.seek Integer(amount), whence

    return 0
  end

  def set_encoding(external, internal=nil, options=undefined)
    case external
    when Encoding
      @external = external
    when String
      @external = nil
    when nil
      if mode_read_only? || @external
        @external = nil
      else
        @external = Encoding.default_external
      end
    else
      @external = nil
      external = StringValue(external)
    end

    if @external.nil? and not external.nil?
      if index = external.index(":")
        internal = external[index+1..-1]
        external = external[0, index]
      end

      if external[3] == ?|
        if encoding = strip_bom
          external = encoding
        else
          external = external[4..-1]
        end
      end

      @external = Encoding.find external
    end

    unless undefined.equal? options
      # TODO: set the encoding options on the IO instance
      if options and not options.kind_of? Hash
        @encoding_options = Rubinius::Type.coerce_to options, Hash, :to_hash
      end
    end

    case internal
    when Encoding
      @internal = nil if @external == internal
    when String
      # do nothing
    when nil
      internal = Encoding.default_internal
    else
      internal = StringValue(internal)
    end

    if internal.kind_of? String
      return self if internal == "-"
      internal = Encoding.find internal
    end

    @internal = internal unless internal && @external == internal

    self
  end

  def read_bom_byte
    read_ios, _, _ = IO.select [self], nil, nil, 0.1
    return getbyte if read_ios
  end

  def strip_bom
    return unless File::Stat.fstat(descriptor).file?

    case b1 = getbyte
    when 0x00
      b2 = getbyte
      if b2 == 0x00
        b3 = getbyte
        if b3 == 0xFE
          b4 = getbyte
          if b4 == 0xFF
            return "UTF-32BE"
          end
          ungetbyte b4
        end
        ungetbyte b3
      end
      ungetbyte b2

    when 0xFF
      b2 = getbyte
      if b2 == 0xFE
        b3 = getbyte
        if b3 == 0x00
          b4 = getbyte
          if b4 == 0x00
            return "UTF-32LE"
          end
          ungetbyte b4
        else
          ungetbyte b3
          return "UTF-16LE"
        end
        ungetbyte b3
      end
      ungetbyte b2

    when 0xFE
      b2 = getbyte
      if b2 == 0xFF
        return "UTF-16BE"
      end
      ungetbyte b2

    when 0xEF
      b2 = getbyte
      if b2 == 0xBB
        b3 = getbyte
        if b3 == 0xBF
          return "UTF-8"
        end
        ungetbyte b3
      end
      ungetbyt b2  # FIXME: syntax error waiting to happen!
    end

    ungetbyte b1
    nil
  end

  ##
  # Returns status information for ios as an object of type File::Stat.
  #
  #  f = File.new("testfile")
  #  s = f.stat
  #  "%o" % s.mode   #=> "100644"
  #  s.blksize       #=> 4096
  #  s.atime         #=> Wed Apr 09 08:53:54 CDT 2003
  def stat
    ensure_open

    File::Stat.fstat descriptor
  end

  ##
  # Returns the current "sync mode" of ios. When sync mode is true,
  # all output is immediately flushed to the underlying operating
  # system and is not buffered by Ruby internally. See also IO#fsync.
  #
  #  f = File.new("testfile")
  #  f.sync   #=> false
  def sync
    ensure_open
    @sync == true
  end

  ##
  # Sets the "sync mode" to true or false. When sync mode is true,
  # all output is immediately flushed to the underlying operating
  # system and is not buffered internally. Returns the new state.
  # See also IO#fsync.
  def sync=(v)
    ensure_open
    @sync = !!v
  end

  ##
  # Reads integer bytes from ios using a low-level read and returns
  # them as a string. Do not mix with other methods that read from
  # ios or you may get unpredictable results. Raises SystemCallError
  # on error and EOFError at end of file.
  #
  #  f = File.new("testfile")
  #  f.sysread(16)   #=> "This is line one"
  #
  def sysread(number_of_bytes, buffer=undefined)
    str = @fd.sysread number_of_bytes
    raise EOFError if str.nil?

    unless undefined.equal? buffer
      StringValue(buffer).replace str
    end

    str
  end

  ##
  # Seeks to a given offset in the stream according to the value
  # of whence (see IO#seek for values of whence). Returns the new offset into the file.
  #
  #  f = File.new("testfile")
  #  f.sysseek(-13, IO::SEEK_END)   #=> 53
  #  f.sysread(10)                  #=> "And so on."
  def sysseek(amount, whence=SEEK_SET)
    ensure_open
    amount = Integer(amount)

    @fd.sysseek amount, whence
  end

  def to_io
    self
  end

  def ftruncate(offset)
    @fd.ftruncate offset
  end

  def truncate(name, offset)
    @fd.truncate name, offset
  end

  ##
  # Returns true if ios is associated with a terminal device (tty), false otherwise.
  #
  #  File.new("testfile").isatty   #=> false
  #  File.new("/dev/tty").isatty   #=> true
  def tty?
    @fd.tty?
  end

  alias_method :isatty, :tty?

  def ttyname
    @fd.ttyname
  end

  def syswrite(data)
    data = String data
    return 0 if data.bytesize == 0

    ensure_open_and_writable
    #    @ibuffer.unseek!(self) unless @sync

    @fd.write(data)
  end

  def ungetbyte(obj)
    ensure_open

    case obj
    when String
      str = obj
    when Integer
      @fd.unget(obj & 0xff)
      return
    when nil
      return
    else
      str = StringValue(obj)
    end

    str.bytes.reverse_each { |byte| @fd.unget byte }

    nil
  end

  def ungetc(obj)
    ensure_open

    case obj
    when String
      str = obj
    when Integer
      @fd.unget(obj)
      return
    when nil
      return
    else
      str = StringValue(obj)
    end

    str.bytes.reverse_each { |byte| @fd.unget byte }

    nil
  end

  def write(data)
    data = String data
    return 0 if data.bytesize == 0

    ensure_open_and_writable

    if !binmode? && external_encoding &&
        external_encoding != data.encoding &&
        external_encoding != Encoding::ASCII_8BIT
      unless data.ascii_only? && external_encoding.ascii_compatible?
        data.encode!(external_encoding)
      end
    end
    data.encode!(@encoding_options) unless (@encoding_options || {}).empty?

    #    if @sync
    @fd.write(data)
    #    else
    #      @ibuffer.unseek! self
    #      bytes_to_write = data.bytesize
    #
    #      while bytes_to_write > 0
    #        bytes_to_write -= @ibuffer.unshift(data, data.bytesize - bytes_to_write)
    #        @ibuffer.empty_to self if @ibuffer.full? or sync
    #      end
    #    end

    data.bytesize
  end

  def write_nonblock(data, exception: true)
    ensure_open_and_writable

    data = String data
    return 0 if data.bytesize == 0

    @fd.write_nonblock(data)
  rescue EAGAINWaitWritable => exc
    raise exc if exception

    return :wait_writable
  end

  def close
    return if closed?
    begin
      flush
    ensure
      @fd.close
    end

    if @pid and @pid != 0
      begin
        Process.wait @pid
      rescue Errno::ECHILD
        # If the child already exited
      end
      @pid = nil
    end

    return nil
  end

  def ensure_open(fd=nil)
    raise IOError, "uninitialized stream" unless @fd
    @fd.ensure_open(fd)
  end

  def ensure_open_and_readable
    ensure_open
    raise IOError, "not opened for reading" if @fd.write_only?
  end

  def ensure_open_and_writable
    ensure_open
    raise IOError, "not opened for writing" if @fd.read_only?
  end

  def get_empty_8bit_buffer
    FileDescriptor.get_empty_8bit_buffer
  end

  def invalid_descriptor?
    descriptor == -1 || descriptor.nil?
  end
  private :invalid_descriptor?

  def emulate_blocking_read
    # Simple wrapper intended to wrap a call to #read_nonblock.
    # Loops forever while waiting for data to become available
    # just like a blocking read, but avoids blocking on the
    # low-level read(2) call. Allows us to catch situations
    # where another thread has closed the FD.
    begin
      yield
    rescue IO::EAGAINWaitReadable
      sleep 0.10
      retry
    rescue EOFError
    end
  end
  private :emulate_blocking_read
end

##
# Implements the pipe returned by IO::pipe.

class IO::BidirectionalPipe < IO

  def set_pipe_info(write)
    @write = write
    @sync = true
  end

  ##
  # Closes ios and flushes any pending writes to the
  # operating system. The stream is unavailable for
  # any further data operations; an IOError is raised
  # if such an attempt is made. I/O streams are
  # automatically closed when they are claimed by
  # the garbage collector.
  #
  # If ios is opened by IO.popen, close sets $?.
  def close
    @write.close unless @write.closed?

    super unless closed?

    nil
  end

  def closed?
    super and @write.closed?
  end

  def close_read
    super
  end

  def close_write
    return if @write.closed?

    @write.close
  end

  # Expand these out rather than using some metaprogramming because it's a fixed
  # set and it's faster to have them as normal methods because then InlineCaches
  # work right.
  #
  def <<(obj)
    @write << obj
  end

  def print(*args)
    @write.print(*args)
  end

  def printf(fmt, *args)
    @write.printf(fmt, *args)
  end

  def putc(obj)
    @write.putc(obj)
  end

  def puts(*args)
    @write.puts(*args)
  end

  def syswrite(data)
    @write.syswrite(data)
  end

  def write(data)
    @write.write(data)
  end

  def write_nonblock(data)
    @write.write_nonblock(data)
  end

end

module Rubinius
  class IOUtility
    # Redefine STDIN, STDOUT & STDERR to use the new IO class. It reopened and redefined
    # all methods used in the bootstrap step. Secondly, update the $std* globals to point
    # to the new objects.

    def self.redefine_io(fd, mode)
      # Note that we use IO.open instead of IO.reopen. The reason is that we reopened the
      # IO class in common/io.rb and overwrote a lot of the methods that were defined in
      # bootstrap/io.rb. So if we try to use IO.reopen on the original IO object, it won't
      # be able to reach those original methods anymore. So, we just pass in the file
      # descriptor integer directly and wrap it up in a new object. The original object
      # will probably get garbage collected but we don't set a finalizer for FDs 0-2 which
      # correspond to STDIN, STDOUT and STDERR so we don't need to worry that they'll get
      # closed out from under us.
      # Hopefully we can find a cleaner way to do this in the future, but for now it's a
      # bit ugly.
      new_io = IO.open(fd)
      new_io.sync = true

      if mode == :read_only
        new_io.force_read_only
      elsif mode == :write_only
        new_io.force_write_only
      end

      return new_io
    end
  end
end
