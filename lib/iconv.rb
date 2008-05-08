# Simple $5 Iconv implementation using FFI

class Iconv

  # Makes all following +attach_function+ calls look in this library
  set_ffi_lib "libiconv"

  attach_function "iconv_open",  :create,  [:string, :string], :pointer
  attach_function "iconv_close", :close, [:pointer],         :int
  attach_function "iconv", :convert, [:pointer, :pointer, :pointer, :pointer, :pointer], :long

  def initialize(to, from)
    @to, @from = to, from
    @handle = Iconv.create to, from
    Errno.handle if @handle.address == -1
  end

  def close
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
    open(to, from) do |cd|
      rest.map { |s| cd.iconv(s) }
    end
  end

  def self.conv(to, from, str)
    iconv(to, from, str).join
  end

  def iconv(str)
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

    oc.write_long output
    l2.write_long os.address

    count = Iconv.convert @handle, l1, ic, l2, oc
    Errno.handle if count == -1

    size = l2.read_int - os.address
    output = os.read_string(size)

    l1.free; l2.free; is.free; ic.free; os.free; oc.free

    return output
  end
end

=begin

input = "Late night hacks rock"

cd = Iconv.new "UTF-16", "ASCII"

out = cd.iconv(input)

c2 = Iconv.new "ASCII", "UTF-16"

same = c2.iconv(out)

p [input, out, same]

=end
