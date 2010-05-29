# Simple $5 Iconv implementation using FFI


class Iconv
  extend FFI::Library

  # Use libiconv if it exists, otherwise get the symbols from the current
  # process.
  ffi_lib ["libiconv", FFI::CURRENT_PROCESS]

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

  def string_value(obj)
    if obj.instance_of? String
      obj
    else
      if obj.respond_to? :to_str
        obj.to_str
      else
        raise TypeError.new "can't convert #{obj.class} into String"
      end
    end
  end

  def initialize(to, from)
    @to, @from = string_value(to), string_value(from)
    @handle = Iconv.create @to, @from
    begin
      Errno.handle if @handle.address == -1
    rescue Errno::EINVAL
      raise InvalidEncoding.new("invalid encoding (#{@to.inspect}, #{@from.inspect})", nil, [@to, @from])
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
          raise e.class.new(e.message, converted, [e.failed] + rest[i + 1..-1])
        end
      end
    end

    converted
  end

  def self.conv(to, from, str)
    begin
      iconv(to, from, str).join
    rescue Failure => e
      # e.success is nil if open throws InvalidEncoding
      # e.success is an Array if #iconv throws any Failure
      raise e.class.new(e.message, (e.success.instance_of? Array) ? e.success.join : e.success, e.failed)
    end
  end

  def get_success(os, l2)
    os.read_string(l2.read_pointer.address - os.address)
  end

  def get_failed(is, ic, l1)
    (is + (l1.read_pointer.address - is.address)).read_string(ic.read_long)
  end

  private :get_success
  private :get_failed

  def iconv(str, start = 0, length = -1)

    start = 0 if not start
    length = -1 if not length

    raise ArgumentError.new("closed iconv") if @closed

    l1 = FFI::MemoryPointer.new(:pointer)
    l2 = FFI::MemoryPointer.new(:pointer)

    ic = FFI::MemoryPointer.new(:long)
    if str then

      if not str.instance_of? String then
        if str.respond_to? :to_str then
          str = str.to_str
        else
          raise TypeError.new "can't convert #{str.class} into String"
        end
      end

      is = FFI::MemoryPointer.new(str.size + 10)
      is.write_string str, str.size

      l1.write_long is.address

      start += str.size if start < 0
      if start < 0 || start >= str.size
        length = 0
      else
        length += str.size + 1 if length < 0
        if length < 0
          length = 0
        else
          length -= start
          if length < 0
            length = 0
          else
            l1.write_long(is.address + start)
            length = str.size if length > str.size
          end
        end
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

    loop do
      oc.write_long output
      l2.write_long os.address

      count = Iconv.convert @handle, l1, ic, l2, oc

      if oc.read_long < 0 || oc.read_long > output then
        raise OutOfRange.new("bug?(output length = #{output - oc.read_long})", get_success(os, l2), get_failed(is, ic, l1))
      end

      if count == -1 then
        begin
          Errno.handle if count == -1
        rescue Errno::EILSEQ => e
          raise IllegalSequence.new(nil, get_success(os, l2), get_failed(is, ic, l1))
        rescue Errno::E2BIG => e
          result += get_success(os, l2)
          next
        rescue Errno::EINVAL => e
          failed = get_failed(is, ic, l1)
          raise InvalidCharacter.new(failed.inspect, get_success(os, l2), failed)
        rescue RuntimeError => e
          raise BrokenLibrary.new(nil, get_success(os, l2), get_failed(is, ic, l1))
        end
      elsif ic.read_long > 0 then
        raise IllegalSequence.new(nil, get_success(os, l2), get_failed(is, ic, l1))
      end

      result += get_success(os, l2)
      break
    end

    result

  ensure
    [l1, l2, is, ic, os, oc].each do |mp|
      mp.free if mp
    end
  end

end
