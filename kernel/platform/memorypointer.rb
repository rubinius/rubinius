module FFI

  ##
  # MemoryPointer is Rubinius's "fat" pointer class. It represents an actual
  # pointer, in C language terms, to an address in memory. They're called
  # fat pointers because the MemoryPointer object is an wrapper around
  # the actual pointer, the Rubinius runtime doesn't have direct access
  # to the raw address.
  #
  # This class is used extensively in FFI usage to interface with various
  # parts of the underlying system. It provides a number of operations
  # for operating on the memory that is pointed to. These operations effectively
  # give Rubinius the cast/read capabilities available in C, but using
  # high level methods.
  #
  # MemoryPointer objects can be put in autorelease mode. In this mode,
  # when the GC cleans up a MemoryPointer object, the memory it points
  # to is passed to free(3), releasing the memory back to the OS.
  #
  # NOTE: MemoryPointer exposes direct, unmanaged operations on any
  # memory. It therefore MUST be used carefully. Reading or writing to
  # invalid address will cause bus errors and segmentation faults.

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
        type = FFI.find_type type
        size = FFI.type_size(type)
      else
        size = type.size
      end

      if count
        total = size * count
      else
        total = size
      end

      ptr = FFI::Platform::POSIX.malloc total
      ptr.total = total
      ptr.type_size = size
      FFI::Platform::POSIX.memset ptr, 0, total if clear

      if block_given?
        begin
          value = yield ptr
        ensure
          ptr.free
        end

        return value
      else
        ptr.autorelease = true
        ptr
      end
    end

    def copy
      other = FFI::Platform::POSIX.malloc total
      other.total = total
      other.type_size = type_size
      FFI::Platform::POSIX.memcpy other, self, total
      other.send :initialize_copy, self
      other
    end

    def address=(address)
      Ruby.primitive :memorypointer_set_address
      raise PrimitiveFailure, "MemoryPointer#address= primitive failed"
    end

    # Indicates how many bytes the chunk of memory that is pointed to takes up.
    attr_accessor :total

    # Indicates how many bytes the type that the pointer is cast as uses.
    attr_accessor :type_size

    # Access the MemoryPointer like a C array, accessing the +which+ number
    # element in memory. The position of the element is calculate from
    # +@type_size+ and +which+. A new MemoryPointer object is returned, which
    # points to the address of the element.
    #
    # Example:
    #   ptr = MemoryPointer.new(:int, 20)
    #   new_ptr = ptr[9]
    #
    # c-equiv:
    #   int *ptr = (int*)malloc(sizeof(int) * 20);
    #   int *new_ptr;
    #   new_ptr = &ptr[9];
    #
    def [](which)
      raise ArgumentError, "unknown type size" unless @type_size
      self + (which * @type_size)
    end

    # Release the memory pointed to back to the OS.
    def free
      self.autorelease = false
      FFI::Platform::POSIX.free(self) unless null?
      #self.class.set_address self, nil
    end

    # Write +obj+ as a C short at the memory pointed to.
    def write_short(obj)
      Ruby.primitive :memorypointer_write_short
      raise PrimitiveFailure, "Unable to write short"
    end

    # Read a C short from the memory pointed to.
    def read_short
      Ruby.primitive :memorypointer_read_short
      raise PrimitiveFailure, "Unable to read short"
    end

    # Write +obj+ as a C int at the memory pointed to.
    def write_int(obj)
      Ruby.primitive :memorypointer_write_int
      raise PrimitiveFailure, "Unable to write integer"
    end

    def read_int(sign=true)
      read_int_signed(sign)
    end

    # Read a C int from the memory pointed to.
    def read_int_signed(sign)
      Ruby.primitive :memorypointer_read_int
      raise PrimitiveFailure, "Unable to read integer"
    end

    # Write +obj+ as a C long at the memory pointed to.
    def write_long(obj)
      Ruby.primitive :memorypointer_write_long
      raise PrimitiveFailure, "Unable to write long"
    end

    # Read a C long from the memory pointed to.
    def read_long
      Ruby.primitive :memorypointer_read_long
      raise PrimitiveFailure, "Unable to read long"
    end

    # Write +obj+ as a C long long at the memory pointed to.
    def write_long_long(obj)
      Ruby.primitive :memorypointer_write_long_long
      raise PrimitiveFailure, "Unable to write long long"
    end

    alias_method :write_int64, :write_long_long

    # Read a C long from the memory pointed to.
    def read_long_long
      Ruby.primitive :memorypointer_read_long_long
      raise PrimitiveFailure, "Unable to read long long"
    end

    alias_method :read_int64, :read_long_long

    def network_order(start, size)
      Ruby.primitive :memorypointer_network_order
      raise PrimitiveFailure, "Unable to convert to network order"
    end

    def read_string_length(len)
      Ruby.primitive :memorypointer_read_string
      raise PrimitiveFailure, "Unable to read string"
    end

    def read_string_to_null
      Ruby.primitive :memorypointer_read_string_to_null
      raise PrimitiveFailure, "Unable to read string to null"
    end

    def read_string(len=nil)
      if len
        read_string_length(len)
      else
        read_string_to_null
      end
    end

    def write_string_length(str, len)
      Ruby.primitive :memorypointer_write_string
      raise PrimitiveFailure, "Unable to write string"
    end

    def write_string(str, len=nil)
      len = str.size unless len

      write_string_length(str, len);
    end

    def read_pointer
      Ruby.primitive :memorypointer_read_pointer
      raise PrimitiveFailure, "Unable to read pointer"
    end

    def write_float(obj)
      Ruby.primitive :memorypointer_write_float
      raise PrimitiveFailure, "Unable to write float"
    end

    def read_float
      Ruby.primitive :memorypointer_read_float
      raise PrimitiveFailure, "Unable to read float"
    end
    
    def write_double(obj)
      Ruby.primitive :memorypointer_write_double
      raise PrimitiveFailure, "Unable to write double"
    end
    
    def read_double
      Ruby.primitive :memorypointer_read_double
      raise PrimitiveFailure, "Unable to read double"
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
      size = FFI.type_size(FFI.find_type type)
      tmp = self
      length.times {
        ary << tmp.send(reader)
        tmp += size
      }
      ary
    end

    def write_array_of_type(type, writer, ary)
      size = FFI.type_size(FFI.find_type type)
      tmp = self
      ary.each do |i|
        tmp.send(writer, i)
        tmp += size
      end
      self
    end

    def get_at_offset(offset, type)
      Ruby.primitive :memorypointer_get_at_offset
      raise PrimitiveFailure, "get_field failed"
    end

    def set_at_offset(offset, type, val)
      Ruby.primitive :memorypointer_set_at_offset
      raise PrimitiveFailure, "set_field failed"
    end

    def inspect
      # Don't have this print the data at the location. It can crash everything.
      "#<#{self.class.name} address=0x#{address.to_s(16)} size=#{total}>"
    end

    def address
      Ruby.primitive :memorypointer_address
      raise PrimitiveFailure, "Unable to find address"
    end

    def null?
      address == 0x0
    end

    def +(value)
      Ruby.primitive :memorypointer_add
      raise PrimitiveFailure, "Unable to add"
    end

    ##
    # If +val+ is true, this MemoryPointer object will call
    # free() on it's address when it is garbage collected.

    def autorelease=(val)
      Ruby.primitive :memorypointer_set_autorelease
      raise PrimitiveFailure, "Unable to change autorelease"
    end
  end
end
