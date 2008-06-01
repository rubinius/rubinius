# Simple $5 Iconv implementation using FFI

class Iconv

  # Makes all following +attach_function+ calls look in this library
  set_ffi_lib "libiconv"

  attach_function "iconv_open",  :create,  [:string, :string], :pointer
  attach_function "iconv_close", :close, [:pointer],         :int
  attach_function "iconv", :convert, [:pointer, :pointer, :pointer, :pointer, :pointer], :long

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
      while x = rest.shift
        begin
          converted << cd.iconv(x)
        rescue Failure => e
          converted << e.success
          raise e.class.new(nil, converted, rest.unshift(e.failed))
        end
      end
    end
    
    converted
  end

  def self.conv(to, from, str)
    begin
      iconv(to, from, str).join
    rescue Failure => e
      raise e.class.new(nil, e.success.join, e.failed)
    end
  end

  def get_success(os, l2)
    os.read_string(l2.read_int - os.address)
  end

  def get_failed(is, ic, l1)
    (is + (l1.read_int - is.address)).read_string(ic.read_long)
  end

  private :get_success
  private :get_failed

  def iconv(str, start = Undefined, length = Undefined)

    raise ArgumentError.new("closed iconv") if @closed

    l1 = MemoryPointer.new(:pointer)
    l2 = MemoryPointer.new(:pointer)

    ic = MemoryPointer.new(:long)
    if str then

      if not str.instance_of? String then
        if str.respond_to? :to_str then
          str = str.to_str
        else
          raise TypeError.new "can't convert #{str.class} into String"
        end
      end

      is = MemoryPointer.new(str.size + 10)
      is.write_string str, str.size

      ic.write_long str.size
      l1.write_long is.address
    else # if str is nil, reset the shift state
      ic.write_long 0
      l1.write_long 0
    end


    # Totally made up metric
    output = 1024
    os = MemoryPointer.new(output)
    oc = MemoryPointer.new(:long)

    result = ""

    loop do
      oc.write_long output
      l2.write_long os.address

      count = Iconv.convert @handle, l1, ic, l2, oc
      if count == -1 then
        begin
          Errno.handle if count == -1
        rescue Errno::EILSEQ => e
          raise IllegalSequence.new(nil, get_success(os, l2), get_failed(is, ic, l1))
        rescue Errno::E2BIG => e
          result += get_success(os, l2)
          next
        rescue Errno::EINVAL => e
          raise InvalidCharacter.new(nil, get_success(os, l2), get_failed(is, ic, l1))
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
