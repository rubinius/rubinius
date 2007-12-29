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
  
  add_typedef TYPE_OBJECT,  :object
  add_typedef TYPE_CHAR,    :char
  add_typedef TYPE_UCHAR,   :uchar
  add_typedef TYPE_SHORT,   :short
  add_typedef TYPE_USHORT,  :ushort
  add_typedef TYPE_INT,     :int
  add_typedef TYPE_UINT,    :uint
  add_typedef TYPE_LONG,    :long
  add_typedef TYPE_ULONG,   :ulong
  add_typedef TYPE_LL,      :long_long
  add_typedef TYPE_ULL,     :ulong_long
  add_typedef TYPE_FLOAT,   :float
  add_typedef TYPE_DOUBLE,  :double
  add_typedef TYPE_PTR,     :pointer
  add_typedef TYPE_VOID,    :void
  add_typedef TYPE_STRING,  :string
  add_typedef TYPE_STATE,   :state
  add_typedef TYPE_STRPTR,  :string_and_pointer
  add_typedef TYPE_STRPTR,  :strptr
  add_typedef TYPE_CHARARR, :char_array
  
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

    func = FFI.create_function nil, name, args, ret

    raise "Unable to create function #{name}" unless func

    metaclass.method_table[mname] = func
    return func
  end
end

class MemoryPointer
  # call-seq:
  #   MemoryPointer.new(num) => MemoryPointer instance of <i>num</i> bytes
  #   MemoryPointer.new(sym) => MemoryPointer instance with number 
  #                             of bytes need by FFI type <i>sym</i>
  #   MemoryPointer.new(sym, count) => MemoryPointer instance with number
  #                             of bytes need by length-<i>count</i> array
  #                             of FFI type <i>sym</i>
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
    size = type.is_a?(Fixnum) ? type : FFI.type_size(type)
    size *= count if count
    ptr = Platform::POSIX.malloc size
    Platform::POSIX.memset ptr, 0, size if clear

    if block_given?
      begin
        yield ptr
      ensure
        ptr.free
      end
    else
      ptr
    end
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
  
  def read_string
    self.class.read_string self
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
  
  def inspect
    "#<MemoryPointer address=0x#{address.to_s(16)}>"
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
  attach_function "ffi_write_float", :write_float, [:pointer, :double], :double
  attach_function "ffi_read_float", :read_float, [:pointer], :double
  attach_function "ffi_read_string", :read_string, [:pointer], :string
  attach_function "ffi_read_pointer", :read_pointer, [:pointer], :pointer
  attach_function "ffi_add_ptr", :add_ptr, [:pointer, :int], :pointer
end

module FFI
  attach_function "ffi_type_size", :get_type_size, [:int], :int

  def self.type_size(type)
    get_type_size(find_type(type))
  end

  class Struct

    attach_function "ffi_get_field", [:pointer, :int, :int], :object
    attach_function "ffi_set_field", [:pointer, :int, :int, :object], :void

    def self.layout(*spec)
      return @layout if spec.empty?
      
      cspec = {}
      i = 0
      
      while i < spec.size
        name = spec[i]
        f = spec[i + 1]
        offset = spec[i + 2]

        code = FFI.find_type(f)
        cspec[name] = [offset, code]
        i += 3
      end
      
      if self != Struct
        @layout = cspec
      end
      
      return cspec
    end
    
    def initialize(ptr, *spec)
      @ptr = ptr
      @cspec = self.class.layout(*spec)
    end
    
    def [](field)
      offset, type = @cspec[field]
      raise "Unknown field #{field}" unless offset
      
      if type == FFI::TYPE_CHARARR
        (@ptr + offset).read_string
      else
        self.class.ffi_get_field(@ptr, offset, type)
      end
    end
    
    def []=(field, val)
      offset, type = @cspec[field]
      raise "Unknown field #{field}" unless offset
      
      self.class.ffi_set_field(@ptr, offset, type, val)
      return val
    end
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

# Define it now so that the rest of platform can use it.
module Platform
end
