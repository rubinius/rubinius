module FFI

  class TypeError < RuntimeError
  end

  TypeDefs = {}

  class << self

    def add_typedef(current, add)
      if current.kind_of? Fixnum
        code = current
      else
        code = FFI::TypeDefs[current]
        raise TypeError, "Unable to resolve type '#{current}'" unless code
      end

      FFI::TypeDefs[add] = code
    end

    def find_type(name)
      code = FFI::TypeDefs[name]
      raise TypeError, "Unable to resolve type '#{name}'" unless code
      return code
    end

    def create_backend(library, name, args, ret)
      Ruby.primitive :nfunc_add
    end

    def create_function(library, name, args, ret)
      i = 0
      tot = args.size
      # We use this instead of map or each because it's really early, map
      # isn't yet available.
      while i < tot
        args[i] = find_type(args[i])
        i += 1
      end
      cret = find_type(ret)

      create_backend(library, name, args, cret)
    end

  end

  ##
  # Converts a Rubinius Object

  add_typedef TYPE_OBJECT,  :object

  ##
  # Converts a char

  add_typedef TYPE_CHAR,    :char

  ##
  # Converts an unsigned char

  add_typedef TYPE_UCHAR,   :uchar

  ##
  # Converts a short

  add_typedef TYPE_SHORT,   :short

  ##
  # Converts an unsigned short

  add_typedef TYPE_USHORT,  :ushort

  ##
  # Converts an int

  add_typedef TYPE_INT,     :int

  ##
  # Converts an unsigned int

  add_typedef TYPE_UINT,    :uint

  ##
  # Converts a long

  add_typedef TYPE_LONG,    :long

  ##
  # Converts an unsigned long

  add_typedef TYPE_ULONG,   :ulong

  ##
  # Converts a long long

  add_typedef TYPE_LL,      :long_long

  ##
  # Converts an unsigned long long

  add_typedef TYPE_ULL,     :ulong_long

  ##
  # Converts a float

  add_typedef TYPE_FLOAT,   :float

  ##
  # Converts a double

  add_typedef TYPE_DOUBLE,  :double

  ##
  # Converts a pointer to opaque data

  add_typedef TYPE_PTR,     :pointer

  ##
  # For when a function has no return value

  add_typedef TYPE_VOID,    :void

  ##
  # Converts NULL-terminated C strings

  add_typedef TYPE_STRING,  :string

  ##
  # Converts the current Rubinius state

  add_typedef TYPE_STATE,   :state

  ##
  # Use strptr when you need to free the result of some operation.

  add_typedef TYPE_STRPTR,  :strptr
  add_typedef TYPE_STRPTR,  :string_and_pointer

  ##
  # Use for a C struct with a char [] embedded inside.

  add_typedef TYPE_CHARARR, :char_array

  TypeSizes = {
    1 => :char,
    2 => :short,
    4 => :int,
    8 => :long_long,
  }

  TypeSizes[:long] = 8 if Rubinius::L64

  def self.size_to_type(size)
    if sz = TypeSizes[size]
      return sz
    end

    # Be like C, use int as the default type size.
    return :char_array
  end

  def self.config(name)
    ::RUBY_CONFIG["rbx.platform.#{name}"]
  end

  def self.config_hash(name)
    vals = { }
    ::RUBY_CONFIG.each do |key,value|
      if(key =~ /rbx\.platform\.#{name}\.(.+)/)
        vals[$1] = value
      end
    end
    vals
  end

end

class Module

  ##
  # Attach C function +name+ to this module.
  #
  # If you want to provide an alternate name for the module function, supply
  # it after the +name+, otherwise the C function name will be used.#
  #
  # After the +name+, the C function argument types are provided as an Array.
  #
  # The C function return type is provided last.

  def attach_function(name, a3, a4, a5=nil)
    if a5
      mname = a3
      args = a4
      ret = a5
    else
      mname = name.to_sym
      args = a3
      ret = a4
    end

    if args.size > 6 then
      raise ArgumentError, "only C functions with up to 6 args may be attached"
    end

    func = FFI.create_function nil, name, args, ret

    raise ArgumentError, "Unable to find function '#{name}' to bind to #{self.name}.#{mname}" unless func

    metaclass.method_table[mname] = func
    return func
  end

end

class MemoryPointer

  # call-seq:
  #   MemoryPointer.new(num) => MemoryPointer instance of <i>num</i> bytes
  #   MemoryPointer.new(sym) => MemoryPointer instance with number
  #                             of bytes need by FFI type <i>sym</i>
  #   MemoryPointer.new(obj) => MemoryPointer instance with number
  #                             of <i>obj.size</i> bytes
  #   MemoryPointer.new(sym, count) => MemoryPointer instance with number
  #                             of bytes need by length-<i>count</i> array
  #                             of FFI type <i>sym</i>
  #   MemoryPointer.new(obj, count) => MemoryPointer instance with number
  #                             of bytes need by length-<i>count</i> array
  #                             of <i>obj.size</i> bytes
  #   MemoryPointer.new(arg) { |p| ... }
  #
  # Both forms create a MemoryPointer instance. The number of bytes to
  # allocate is either specified directly or by passing an FFI type, which
  # specifies the number of bytes needed for that type.
  #
  # The form without a block returns the MemoryPointer instance. The form
  # with a block yields the MemoryPointer instance and frees the memory
  # when the block returns. The value returned is the value of the block.

  def self.new(type, count=nil, clear=true)
    if type.kind_of? Fixnum
      size = type
    elsif type.kind_of? Symbol
      size = FFI.type_size(type)
    else
      size = type.size
    end

    if count
      total = size * count
    else
      total = size
    end

    ptr = Platform::POSIX.malloc total
    ptr.total = total
    ptr.type_size = size
    Platform::POSIX.memset ptr, 0, total if clear

    if block_given?
      begin
        value = yield ptr

        return value
      ensure
        ptr.free
      end
    else
      ptr
    end
  end

  attr_accessor :total
  attr_accessor :type_size

  def [](which)
    raise ArgumentError, "unknown type size" unless @type_size
    self + (which * @type_size)
  end

  def free
    Platform::POSIX.free self
  end

  def write_int(obj)
    self.class.write_int self, Integer(obj)
  end

  def read_int
    self.class.read_int self
  end

  def write_long(obj)
    self.class.write_long self, Integer(obj)
  end

  def read_long
    self.class.read_long self
  end

  def read_string(len=nil)
    if len
      self.class.read_string_length self, len
    else
      self.class.read_string self
    end
  end

  def write_string(str, len=nil)
    len = str.size unless len

    self.class.write_string self, str, len
  end

  def read_pointer
    self.class.read_pointer self
  end

  def write_float(obj)
    # TODO: ffi needs to be fixed for passing [:pointer, double]
    #       when :pointer is a (double *)
    self.class.write_float self, Float(obj)
  end

  def read_float
    self.class.read_float self
  end

  def read_array_of_int(length)
    read_array_of_type(:int, :read_int, length)
  end

  def write_array_of_int(ary)
    write_array_of_type(:int, :write_int, ary)
  end

  def read_array_of_long(length)
    read_array_of_type(:long, :read_long, length)
  end

  def write_array_of_long(ary)
    write_array_of_type(:long, :write_long, ary)
  end

  def read_array_of_type(type, reader, length)
    ary = []
    size = FFI.type_size(type)
    tmp = self
    length.times {
      ary << tmp.send(reader)
      tmp += size
    }
    ary
  end

  def write_array_of_type(type, writer, ary)
    size = FFI.type_size(type)
    tmp = self
    ary.each {|i|
      tmp.send(writer, i)
      tmp += size
    }
    self
  end

  def inspect
    "#<MemoryPointer address=0x#{address.to_s(16)} size=#{total} data=#{read_string(total).inspect}>"
  end

  def address
    self.class.address self
  end

  def null?
    address == 0x0
  end

  def +(value)
    self.class.add_ptr(self, value)
  end

  attach_function "ffi_address", :address, [:pointer], :int
  attach_function "ffi_write_int", :write_int, [:pointer, :int], :int
  attach_function "ffi_read_int", :read_int, [:pointer], :int
  attach_function "ffi_write_long", :write_long, [:pointer, :long], :long
  attach_function "ffi_read_long", :read_long, [:pointer], :long
  attach_function "ffi_write_float", :write_float, [:pointer, :double], :double
  attach_function "ffi_read_float", :read_float, [:pointer], :double
  attach_function "ffi_read_string", :read_string, [:pointer], :string
  attach_function "ffi_read_string_length", :read_string_length, [:state, :pointer, :int], :object
  attach_function "memcpy", :write_string, [:pointer, :string, :int], :void
  attach_function "ffi_read_pointer", :read_pointer, [:pointer], :pointer
  attach_function "ffi_add_ptr", :add_ptr, [:pointer, :int], :pointer
end

module FFI

  attach_function "ffi_type_size", :get_type_size, [:int], :int

  def self.type_size(type)
    get_type_size(find_type(type))
  end

end

class FFI::Struct

  attr_reader :pointer

  attach_function "ffi_get_field", [:pointer, :int, :int], :object
  attach_function "ffi_set_field", [:pointer, :int, :int, :object], :void

  def self.layout(*spec)
    return @layout if spec.size == 0

    cspec = {}
    i = 0

    @size = 0

    while i < spec.size
      name = spec[i]
      f = spec[i + 1]
      offset = spec[i + 2]

      code = FFI.find_type(f)
      cspec[name] = [offset, code]
      ending = offset + FFI.type_size(f)
      @size = ending if @size < ending

      i += 3
    end

    @layout = cspec unless self == FFI::Struct

    return cspec
  end

  def self.config(base, *fields)
    @size = 0
    cspec = {}

    fields.each do |field|
      offset = Rubinius::RUBY_CONFIG["#{base}.#{field}.offset"]
      size   = Rubinius::RUBY_CONFIG["#{base}.#{field}.size"]
      type   = Rubinius::RUBY_CONFIG["#{base}.#{field}.type"]

      type = if type then
               type.intern
             else
               FFI.size_to_type type
             end

      code = FFI.find_type type

      cspec[field] = [offset, code]

      ending = offset + size
      @size = ending if @size < ending
    end

    @layout = cspec

    return cspec
  end

  def self.size
    @size
  end

  def size
    self.class.size
  end

  def initialize(pointer = nil, *spec)
    if pointer then
      @pointer = pointer
    else
      @pointer = MemoryPointer.new size
    end

    @cspec = self.class.layout(*spec)
  end

  def free
    @pointer.free
  end

  def [](field)
    offset, type = @cspec[field]
    raise "Unknown field #{field}" unless offset

    if type == FFI::TYPE_CHARARR
      (@pointer + offset).read_string
    else
      self.class.ffi_get_field(@pointer, offset, type)
    end
  end

  def []=(field, val)
    offset, type = @cspec[field]
    raise "Unknown field #{field}" unless offset

    self.class.ffi_set_field(@pointer, offset, type, val)
    return val
  end

end

class NativeFunction

  # The *args means the primitive handles it own argument count checks.
  def call(*args)
    Ruby.primitive :nfunc_call_object
  end

  class Variable
    def initialize(library, name, a2, a3=nil)
      if a3
        @ret = a3
        @static_args = a2
      else
        @ret = a2
        @static_args = nil
      end

      @library = library
      @name = name
      @functions = {}
    end

    def find_function(at)
      if @static_args
        at = @static_args + at
      end

      if func = @functions[at]
        return func
      end

      func = FFI.create_function @library, @name, at, @ret
      @functions[at] = func
      return func
    end

    def [](*args)
      find_function(args)
    end

    def call(at, *args)
      find_function(at).call(*args)
    end
  end
end

#++
# Define it now so that the rest of platform can use it.
#--

module Platform
end

