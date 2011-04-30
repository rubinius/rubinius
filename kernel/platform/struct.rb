module FFI
  ##
  # Represents a C struct as ruby class.

  class Struct

    class InlineArray
      def initialize(type, ptr)
        @pointer = ptr
        @type = type.element_type
        @size = type.size
      end

      attr_reader :size

      def [](idx)
        if idx >= @size
          raise ArgumentError, "index out of range (#{idx} >= #{@size})"
        end

        @pointer.get_at_offset(idx * @size, @type)
      end

      def []=(idx, val)
        if idx >= @size
          raise ArgumentError, "index out of range (#{idx} >= #{@size})"
        end

        @pointer.set_at_offset(idx * @size, @type, val)
      end

      def each
        @size.times do |ele|
          yield @pointer.get_at_offset(ele * @size, @type)
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
        if idx = str.index(0)
          return str[0, idx]
        end

        return str
      end

      alias_method :to_str, :to_s
    end

    def self.find_nested_parent
      path = self.name.split("::")
      path.pop # remove ourself

      mod = Object

      begin
        path.each { |c| mod = mod.const_get(c) }
      rescue NameError
        # bail.
        return nil
      end

      return mod if mod.respond_to?(:find_type)

      nil
    end

    attr_reader :pointer

    def self.layout(*spec)
      return @layout if spec.size == 0

      # Pick up a enclosing FFI::Library
      @enclosing_module = find_nested_parent

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
        else
          if @enclosing_module
            type_code = @enclosing_module.find_type(f)
          end

          type_code ||= FFI.find_type(f)

          type = type_code
          element_size = FFI.type_size(type_code)
        end

        offset = spec[i + 2]

        if offset.kind_of?(Fixnum)
          i += 3
        else
          offset = @size

          mod = offset % element_size
          unless mod == 0
            # we need to align it.
            offset += (element_size - mod)
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
        offset = Rubinius::Config["#{base}.#{field}.offset"]
        size   = Rubinius::Config["#{base}.#{field}.size"]
        type   = Rubinius::Config["#{base}.#{field}.type"]
        type   = type ? type.to_sym : FFI.size_to_type(size)

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

    def self.offset_of(name)
      offset, type = @cspec[name]
      return offset
    end

    def offset_of(name)
      offset, type = @cspec[name]
      return offset
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

    def to_ptr
      @pointer
    end

    def free
      @pointer.free
    end

    def initialize_copy(ptr)
      @pointer = ptr.pointer.dup
    end

    def []=(field, val)
      offset, type = @cspec[field]
      raise "Unknown field #{field}" unless offset

      case type
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
      when FFI::Type::Array
        type.implementation.new(type, @pointer + offset)
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

  end
end
