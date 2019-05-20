##
# A Foreign Function Interface used to bind C libraries to ruby.

module Rubinius
  module FFI

    #  Specialised error classes
    class TypeError < RuntimeError; end

    class NotFoundError < RuntimeError; end

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

    # Represents a C enum.
    class Enum
      attr_reader :tag
      attr_reader :kv_map

      def initialize(info, tag=nil)
        @tag = tag
        @kv_map = Hash.new

        unless info.nil?
          last_cst = nil
          value = 0
          info.each do |i|
            case i
            when Symbol
              @kv_map[i] = value
              last_cst = i
              value += 1
            when Integer
              @kv_map[last_cst] = i
              value = i + 1
            end
          end
        end
      end

      private :initialize

      def inspect
        "#<%s:0x%x %s=>%s>" % [self.class, self.object_id, @tag, @kv_map.inspect]
      end

      def anonym?
        @tag.nil?
      end

      def values
        @kv_map.values
      end

      def symbols
        @kv_map.keys
      end

      def [](sym)
        @kv_map[sym]
      end

      def symbol(value)
        key, val = @kv_map.detect { |key, val| val == value }
        key
      end
    end

    # It's a class to be compat with the ffi gem.
    class Type
      class Array
        def initialize(element_type, size, impl_class=nil)
          @element_type = element_type
          @size = size
          @implementation = impl_class
        end

        private :initialize

        attr_reader :element_type
        attr_reader :size
        attr_reader :implementation
      end

      class StructByValue
        def initialize(struct)
          @implementation = struct
        end

        private :initialize

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

    ##
    # Namespace for holding platform-specific C constants.

    module Platform
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

    ##
    # Represents a C struct as ruby class.

    class Struct
      class InlineArray
        include Enumerable

        def initialize(type, ptr)
          @pointer = ptr
          @type = type.element_type
          @size = type.size
        end

        private :initialize

        attr_reader :size

        def [](idx)
          if idx >= @size
            raise ArgumentError, "index out of range (#{idx} >= #{@size})"
          end

          @pointer.get_at_offset(idx * @type, @type)
        end

        def []=(idx, val)
          if idx >= @size
            raise ArgumentError, "index out of range (#{idx} >= #{@size})"
          end

          @pointer.set_at_offset(idx * @type, @type, val)
        end

        def each
          @size.times do |ele|
            yield @pointer.get_at_offset(ele * @type, @type)
          end
        end

        def to_a
          ary = []
          each { |x| ary << x }
          ary
        end

        def to_ptr
          @pointer
        end
      end

      class InlineCharArray < InlineArray
        def to_s
          str = @pointer.read_string(@size)

          # NULL clamp this, to be the same as the ffi gem.
          if idx = str.index("\0")
            return str[0, idx]
          end

          return str
        end

        alias_method :to_str, :to_s
        alias_method :inspect, :to_s
      end

      def self.find_nested_parent
        return nil if self.name.nil?

        path = self.name.split("::")
        path.pop # remove ourself

        mod = Object

        begin
          path.each { |c| mod = mod.const_get(c) }
        rescue NameError
          # bail.
          return nil
        end

        mod.respond_to?(:find_type) ? mod : nil
      end

      attr_reader :pointer

      def self.layout(*spec)
        return @layout if spec.size == 0

        # Pick up a enclosing FFI::Library
        @enclosing_module ||= find_nested_parent

        cspec = Rubinius::LookupTable.new
        i = 0

        @size = 0
        @members = []

        while i < spec.size
          name = spec[i]
          @members << name

          f = spec[i + 1]

          type_code = nil

          if f.kind_of? Array
            ary_type = f[0]
            ary_size = f[1]

            if @enclosing_module
              type_code = @enclosing_module.find_type(ary_type)
            end

            type_code ||= FFI.find_type(ary_type)
            type_size = FFI.type_size(type_code)

            case type_code
            when TYPE_CHAR, TYPE_UCHAR
              klass = InlineCharArray
            else
              klass = InlineArray
            end

            type = FFI::Type::Array.new(type_code, ary_size, klass)
            element_size = type_size * ary_size
          elsif f.kind_of?(Class) and (f < FFI::Struct || f < FFI::Union)
            type = FFI::Type::StructByValue.new(f)
            element_size = type_size = f.size
          else
            if @enclosing_module
              type_code = @enclosing_module.find_type(f) rescue nil
            end

            type_code ||= FFI.find_type(f)

            type = type_code
            element_size = type_size = FFI.type_size(type_code)
          end

          offset = spec[i + 2]

          if offset.kind_of?(Fixnum)
            i += 3
          else
            if self < FFI::Union
              offset = 0
            else
              offset = @size

              mod = offset % type_size
              unless mod == 0
                # we need to align it.
                offset += (type_size - mod)
              end
            end

            i += 2
          end

          cspec[name] = [offset, type]
          ending = offset + element_size
          @size = ending if @size < ending
        end

        @layout = cspec unless self == FFI::Struct

        return cspec
      end

      def self.config(base, *fields)
        @size = Rubinius::Config["#{base}.sizeof"]
        cspec = Rubinius::LookupTable.new

        fields.each do |field|
          offset         = Rubinius::Config["#{base}.#{field}.offset"]
          element_size   = Rubinius::Config["#{base}.#{field}.size"]
          type           = Rubinius::Config["#{base}.#{field}.type"]
          type           = type ? type.to_sym : FFI.size_to_type(size)

          type_code = FFI.find_type type

          if :char_array == type
            type_code = FFI::Type::Array.new(FFI.find_type(:char), element_size, InlineCharArray)
          end

          cspec[field] = [offset, type_code]
          ending = offset + element_size
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

      def self.offset_of(name)
        @layout[name].first
      end

      def offset_of(name)
        @cspec[name].first
      end

      def self.offsets
        members.map do |member|
          [member, @layout[member].first]
        end
      end

      def offsets
        members.map do |member|
          [member, @cspec[member].first]
        end
      end

      def self.members
        @members
      end

      def members
        self.class.members
      end

      def initialize(pointer=nil, *spec)
        @cspec = self.class.layout(*spec)

        if pointer
          @pointer = pointer
        else
          @pointer = MemoryPointer.new size
        end
      end

      private :initialize

      def to_ptr
        @pointer
      end

      def free
        @pointer.free
      end

      def ==(other)
        return false unless other.is_a?(self.class)
        @pointer == other.pointer
      end

      def initialize_copy(ptr)
        @pointer = ptr.pointer.dup
      end

      private :initialize

      def []=(field, val)
        offset, type = @cspec[field]
        raise "Unknown field #{field}" unless offset

        case type
        when Fixnum
          @pointer.set_at_offset(offset, type, val)
        when FFI::Type::Array
          if type.implementation == InlineCharArray
            (@pointer + offset).write_string StringValue(val), type.size
            return val
          end

          raise TypeError, "Unable to set inline array"
        when Rubinius::NativeFunction
          @pointer.set_at_offset(offset, FFI::TYPE_PTR, val)
        else
          @pointer.set_at_offset(offset, type, val)
        end

        return val
      end

      def [](field)
        offset, type = @cspec[field]
        raise "Unknown field #{field}" unless offset

        case type
        when FFI::TYPE_CHARARR
          (@pointer + offset).read_string
        when Fixnum
          @pointer.get_at_offset(offset, type)
        when FFI::Type::Array
          type.implementation.new(type, @pointer + offset)
        when FFI::Type::StructByValue
          type.implementation.new(@pointer + offset)
        when Rubinius::NativeFunction
          ptr = @pointer.get_at_offset(offset, FFI::TYPE_PTR)
          if ptr
            FFI::Function.new(type.return_type, type.argument_types, ptr)
          else
            nil
          end
        else
          @pointer.get_at_offset(offset, type)
        end
      end

      def values
        members.map { |m| self[m] }
      end

      def null?
        @pointer == FFI::Pointer::NULL
      end
    end

    ##
    # Represents a C union as ruby class.

    class Union < Struct
    end
  end
end
