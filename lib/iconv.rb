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

  def initialize(to, from)
    @to, @from = to, from
    @handle = Iconv.create to, from
    Errno.handle if @handle.address == -1
    @closed = false
  end

  def close
    return if @closed
    @closed = true
    Errno.handle if Iconv.close(@handle) != 0
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
    l1 = MemoryPointer.new(:pointer)
    l2 = MemoryPointer.new(:pointer)

    is = MemoryPointer.new(str.size + 10)
    ic = MemoryPointer.new(:long)

    ic.write_long str.size
    is.write_string str, str.size
    l1.write_long is.address

    # Totally made up metric
    output = 1024
    os = MemoryPointer.new(output)
    oc = MemoryPointer.new(:long)

    result = ""

    begin
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
    ensure
      l1.free; l2.free; is.free; ic.free; os.free; oc.free
    end

    return result
  end

end
