# Simple $5 Iconv implementation using FFI


class Iconv
  extend FFI::Library

  # Use libiconv if it exists, otherwise get the symbols from the current
  # process.
  ffi_lib ["libiconv.2.dylib", "libiconv.so.2", "libiconv", FFI::CURRENT_PROCESS]

  attach_function :create,  "iconv_open", [:string, :string], :pointer
  attach_function :close,   "iconv_close", [:pointer], :int
  attach_function :convert, "iconv",
                            [:pointer, :pointer, :pointer, :pointer, :pointer], :long

  module Failure
    attr_reader :success
    attr_reader :failed

    def initialize(mesg, success, failed)
      super(mesg)
      @success = success
      @failed = failed
    end

    def inspect()
      "<##{self.class}: #{success.inspect}, #{failed.inspect}>"
    end
  end

  class BrokenLibrary < RuntimeError
    include Failure
  end

  class IllegalSequence < ArgumentError
    include Failure
  end

  class InvalidCharacter < ArgumentError
    include Failure
  end

  class InvalidEncoding < ArgumentError
    include Failure
  end

  class OutOfRange < RuntimeError
    include Failure
  end

  def initialize(to, from)
    @to = StringValue(to)
    @from = StringValue(from)

    @handle = Iconv.create @to, @from

    begin
      Errno.handle if @handle.address == -1
    rescue Errno::EINVAL
      raise InvalidEncoding.new("invalid encoding (#{@to.inspect}, #{@from.inspect})",
                                nil, [@to, @from])
    end

    @closed = false
  end

  def close
    return if @closed

    begin
      iconv nil
    ensure
      @closed = true
      Errno.handle if Iconv.close(@handle) != 0
    end
  end

  def self.open(to, from)
    cd = new(to, from)

    return cd unless block_given?

    begin
      yield cd
    ensure
      cd.close
    end
  end

  def self.iconv(to, from, *rest)
    converted = []

    open(to, from) do |cd|
      rest.each_with_index do |x, i|
        begin
          converted << cd.iconv(x)
        rescue Failure => e
          converted << e.success
          raise e.class.new(e.message, converted, [e.failed] + rest[i + 1..-1]), nil, e
        end
      end
    end

    converted
  end

  def self.conv(to, from, str)
    begin
      iconv(to, from, str).join
    rescue Failure => e
      success = (e.success.instance_of? Array) ? e.success.join : e.success
      raise e.class.new(e.message, success, e.failed), nil, e
    end
  end

  def get_failed(is, ic, l1)
    (is + (l1.read_pointer.address - is.address)).read_string(ic.read_long)
  end

  private :get_failed

  def iconv(str, start=nil, length=nil)
    raise ArgumentError, "closed iconv" if @closed

    l1 = FFI::MemoryPointer.new(:pointer)
    l2 = FFI::MemoryPointer.new(:pointer)

    ic = FFI::MemoryPointer.new(:long)

    if str
      str = StringValue(str)
      str_size = str.size

      # To deal with people passing in nil's
      start  = 0        unless start
      length = str_size unless length

      is = FFI::MemoryPointer.new(str_size + 10)
      is.write_string str, str_size

      address_ptr = is.address

      # Negative start is an offset from the end
      start += str_size if start < 0

      address_ptr += start

      l1.write_long address_ptr

      # Make sure we don't use a too long length
      # A negative length means we should go on until
      # the end of the string
      if length < 0 || str_size - start < length
        length = str_size - start
      end

      # Make sure we don't use an invalid start
      if start < 0 || start >= str_size
        start  = 0
        length = 0
      end

      ic.write_long length

    else # if str is nil, reset the shift state
      ic.write_long 0
      l1.write_long 0
    end

    # Totally made up metric
    output = 1024
    os = FFI::MemoryPointer.new(output)
    oc = FFI::MemoryPointer.new(:long)

    result = ""

    while true
      oc.write_long output
      l2.write_long os.address

      count = Iconv.convert @handle, l1, ic, l2, oc

      input_left  = ic.read_long
      buffer_left = oc.read_long

      if 0 > buffer_left or buffer_left > output
        raise OutOfRange.new("bug?(output length = #{used})",
                             result, get_failed(is, ic, l1))
      end

      used = output - buffer_left
      last_output = os.read_string(used)

      result << last_output

      # We ignore the error if all the input was consumed.
      if count == -1 and input_left > 0
        begin
          Errno.handle
        rescue Errno::EILSEQ => e
          raise IllegalSequence.new("illegal character sequence",
                                    result, get_failed(is, ic, l1))
        rescue Errno::E2BIG => e
          # ignore
        rescue Errno::EINVAL => e
          failed = get_failed(is, ic, l1)
          raise InvalidCharacter.new(failed.inspect, result, failed)
        rescue RuntimeError => e
          raise BrokenLibrary.new("iconv(2) is broken",
                                  result, get_failed(is, ic, l1))
        end
      end

      break if input_left <= 0
    end

    result

  ensure
    [l1, l2, is, ic, os, oc].each do |mp|
      mp.free if mp
    end
  end

end
