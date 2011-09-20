class IO
  def self.binread(file, *arg)
    unless arg.size < 3
      raise ArgumentError, "wrong number of arguments (#{1+arg.size} for 1..3)"
    end

    File.open(file,"rb") do |f|
      f.read(*arg)
    end
  end

  def self.read(name, length_or_options=undefined, offset=0, options=nil)
    name = Rubinius::Type.coerce_to_path name
    mode = "r"

    if length_or_options.equal? undefined
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
      mode = options[:mode] || "r"
    end

    # Detect pipe mode
    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io # child process
    else
      io = File.new(name, mode)
    end

    str = nil
    begin
      io.seek(offset) unless offset == 0

      if length.equal?(undefined)
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

  class StreamCopier
    def initialize(from, to, length, offset)
      @length = length
      @offset = offset

      @from_io, @from = to_io(from, "rb")
      @to_io, @to = to_io(to, "wb")

      @method = read_method @from
    end

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
      @from.ensure_open_and_readable
      @to.ensure_open_and_writable

      saved_pos = @from.pos if @from_io

      @from.seek @offset, IO::SEEK_CUR if @offset

      size = rest = @length ? @length : 16384
      bytes = 0

      begin
        while data = @from.send(@method, size, "")
          @to.write data
          bytes += data.size
          rest -= data.size

          break unless rest > 0
        end
      rescue EOFError
        # done reading
      end

      @to.flush
      return bytes
    ensure
      if @from_io
        @from.pos = saved_pos if @offset
      else
        @from.close
      end

      @to.close unless @to_io
    end
  end

  def self.copy_stream(from, to, max_length=nil, offset=nil)
    StreamCopier.new(from, to, max_length, offset).run
  end
end
