##
# A Foreign Function Interface used to bind C libraries to ruby.

module Rubinius
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
      Rubinius.primitive :nativefunction_type_size

      case type
      when Symbol
        return type_size(find_type(type))
      when Rubinius::NativeFunction
        return type_size(TYPE_PTR)
      when FFI::Enum
        return type_size(TYPE_ENUM)
      end

      raise PrimitiveFailure, "FFI.type_size primitive failed: #{type}"
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
      Rubinius::Config.section section do |key, value|
        vals[key.substring(section.size, key.length)] = value
      end
      vals
    end

    def errno
      FFI::Platform::POSIX.errno
    end

    # Convenience method for determining if a function call succeeded or failed
    def call_failed?(return_code)
      return_code == -1
    end
  end

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
  add_typedef TYPE_USHORT,  :mode_t
  add_typedef TYPE_USHORT,  :nlink_t

  # Converts an int
  add_typedef TYPE_INT,     :int
  add_typedef TYPE_INT,     :dev_t
  add_typedef TYPE_INT,     :blksize_t
  add_typedef TYPE_INT,     :time_t

  # Converts an unsigned int
  add_typedef TYPE_UINT,    :uint
  add_typedef TYPE_UINT,    :uid_t
  add_typedef TYPE_UINT,    :gid_t

  # Converts a long
  add_typedef TYPE_LONG,    :long
  add_typedef TYPE_LONG,    :ssize_t

  # Converts an unsigned long
  add_typedef TYPE_ULONG,   :ulong

  # Converts a size_t
  add_typedef TYPE_ULONG,   :size_t

  # Converts a long long
  add_typedef TYPE_LL,      :long_long
  add_typedef TYPE_LL,      :blkcnt_t
  add_typedef TYPE_LL,      :off_t

  # Converts an unsigned long long
  add_typedef TYPE_ULL,     :ulong_long
  add_typedef TYPE_ULL,     :ino64_t

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

  # Converts a varargs argument
  add_typedef TYPE_VARARGS, :varargs

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

    class StructByValue
      def initialize(struct)
        @implementation = struct
      end

      attr_reader :implementation
    end

    Struct = StructByValue

    CHAR    = TYPE_CHAR
    UCHAR   = TYPE_UCHAR
    BOOL    = TYPE_BOOL
    SHORT   = TYPE_SHORT
    USHORT  = TYPE_USHORT
    INT     = TYPE_INT
    UINT    = TYPE_UINT
    LONG    = TYPE_LONG
    ULONG   = TYPE_ULONG
    LL      = TYPE_LL
    ULL     = TYPE_ULL
    FLOAT   = TYPE_FLOAT
    DOUBLE  = TYPE_DOUBLE
    PTR     = TYPE_PTR
    VOID    = TYPE_VOID
    STRING  = TYPE_STRING
    STRPTR  = TYPE_STRPTR
    CHARARR = TYPE_CHARARR
    ENUM    = TYPE_ENUM
    VARARGS = TYPE_VARARGS
  end
end

##
# Namespace for holding platform-specific C constants.

module FFI::Platform
  case
  when Rubinius.windows?
    LIBSUFFIX = "dll"
    IS_WINDOWS = true
    OS = 'windows'
  when Rubinius.darwin?
    LIBSUFFIX = "dylib"
    IS_WINDOWS = false
    OS = 'darwin'
  else
    LIBSUFFIX = "so"
    IS_WINDOWS = false
    OS = 'linux'
  end

  ARCH = Rubinius::CPU

  # ruby-ffi compatible
  LONG_SIZE = Rubinius::SIZEOF_LONG * 8
  ADDRESS_SIZE = Rubinius::WORDSIZE

  def self.windows?
    Rubinius.windows?
  end

  def self.mac?
    Rubinius.darwin?
  end

  def self.unix?
    ! windows?
  end
end
end
