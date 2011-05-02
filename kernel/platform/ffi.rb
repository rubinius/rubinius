##
# A Foreign Function Interface used to bind C libraries to ruby.

module FFI

  #  Specialised error classes
  class TypeError < RuntimeError; end

  class NotFoundError < RuntimeError; end

  # Shorthand for the current process, i.e. all code that
  # the process image itself contains. In addition to the
  # Rubinius codebase, this also includes libc etc.
  #
  # Use this constant instead of nil directly.
  #
  CURRENT_PROCESS = nil

  TypeDefs = Rubinius::LookupTable.new

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

    ##
    # Given a +type+ as a number, indicate how many bytes that type
    # takes up on this platform.

    def type_size(type)
      Ruby.primitive :nativefunction_type_size

      case type
      when Symbol
        return type_size(find_type(type))
      when Rubinius::NativeFunction
        return type_size(TYPE_PTR)
      end

      raise PrimitiveFailure, "Unable to find type size for #{type}"
    end

    def size_to_type(size)
      if sz = TypeSizes[size]
        return sz
      end

      # Be like C, use int as the default type size.
      return :int
    end

    def config(name)
      Rubinius::Config["rbx.platform.#{name}"]
    end

    def config_hash(name)
      vals = {}
      section = "rbx.platform.#{name}."
      Rubinius::Config.section section do |key,value|
        vals[key.substring(section.size, key.length)] = value
      end
      vals
    end

  end

  # Converts a Rubinius Object
  add_typedef TYPE_OBJECT,  :object

  # Converts a char
  add_typedef TYPE_CHAR,    :char

  # Converts an unsigned char
  add_typedef TYPE_UCHAR,   :uchar

  # The C++ boolean type
  add_typedef TYPE_BOOL,    :bool

  # Converts a short
  add_typedef TYPE_SHORT,   :short

  # Converts an unsigned short
  add_typedef TYPE_USHORT,  :ushort

  # Converts an int
  add_typedef TYPE_INT,     :int

  # Converts an unsigned int
  add_typedef TYPE_UINT,    :uint

  # Converts a long
  add_typedef TYPE_LONG,    :long

  # Converts an unsigned long
  add_typedef TYPE_ULONG,   :ulong

  # Converts a size_t
  add_typedef TYPE_ULONG,   :size_t

  # Converts a long long
  add_typedef TYPE_LL,      :long_long

  # Converts an unsigned long long
  add_typedef TYPE_ULL,     :ulong_long

  # Converts a float
  add_typedef TYPE_FLOAT,   :float

  # Converts a double
  add_typedef TYPE_DOUBLE,  :double

  # Converts a pointer to opaque data
  add_typedef TYPE_PTR,     :pointer

  # For when a function has no return value
  add_typedef TYPE_VOID,    :void

  # Converts NULL-terminated C strings
  add_typedef TYPE_STRING,  :string

  # Converts the current Rubinius state
  add_typedef TYPE_STATE,   :state

  # Use strptr when you need to free the result of some operation.
  add_typedef TYPE_STRPTR,  :strptr
  add_typedef TYPE_STRPTR,  :string_and_pointer

  # Use for a C struct with a char [] embedded inside.
  add_typedef TYPE_CHARARR, :char_array

  # A set of unambigious integer types
  add_typedef TYPE_CHAR,   :int8
  add_typedef TYPE_UCHAR,  :uint8
  add_typedef TYPE_SHORT,  :int16
  add_typedef TYPE_USHORT, :uint16
  add_typedef TYPE_INT,    :int32
  add_typedef TYPE_UINT,   :uint32

  if Rubinius::L64
    add_typedef TYPE_LONG,  :int64
    add_typedef TYPE_ULONG, :uint64
  else
    add_typedef TYPE_LL,    :int64
    add_typedef TYPE_ULL,   :uint64
  end

  TypeSizes = Rubinius::LookupTable.new
  TypeSizes[1] = :char
  TypeSizes[2] = :short
  TypeSizes[4] = :int
  TypeSizes[8] = Rubinius::L64 ? :long : :long_long

  # Load all the platform dependent types

  Rubinius::Config.section("rbx.platform.typedef.") do |key, value|
    add_typedef(find_type(value.to_sym), key.substring(21, key.length).to_sym)
  end

  # It's a class to be compat with the ffi gem.
  class Type
    class Array
      def initialize(element_type, size, impl_class=nil)
        @element_type = element_type
        @size = size
        @implementation = impl_class
      end

      attr_reader :element_type
      attr_reader :size
      attr_reader :implementation
    end
  end
end

##
# Namespace for holding platform-specific C constants.

module FFI::Platform
end

