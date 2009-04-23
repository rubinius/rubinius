# depends on: ffi.rb

module FFI
  ##
  # Represents a C struct as ruby class.

  class Struct

    attr_reader :pointer

    def self.layout(*spec)
      return @layout if spec.size == 0

      cspec = Rubinius::LookupTable.new
      i = 0

      @size = 0

      while i < spec.size
        name = spec[i]
        f = spec[i + 1]
        offset = spec[i + 2]

        code = FFI.find_type(f)
        cspec[name] = [offset, code]
        ending = offset + FFI.type_size(code)
        @size = ending if @size < ending

        i += 3
      end

      @layout = cspec unless self == FFI::Struct

      return cspec
    end

    def self.config(base, *fields)
      @size = Rubinius::RUBY_CONFIG["#{base}.sizeof"]
      cspec = Rubinius::LookupTable.new

      fields.each do |field|
        offset = Rubinius::RUBY_CONFIG["#{base}.#{field}.offset"]
        size   = Rubinius::RUBY_CONFIG["#{base}.#{field}.size"]
        type   = Rubinius::RUBY_CONFIG["#{base}.#{field}.type"]
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

    def initialize(pointer = nil, *spec)
      @cspec = self.class.layout(*spec)

      if pointer then
        @pointer = pointer
      else
        @pointer = MemoryPointer.new size
      end
    end

    def free
      @pointer.free
    end

    def initialize_copy(ptr)
      @pointer = ptr.pointer.dup
    end

    def [](field)
      offset, type = @cspec[field]
      raise "Unknown field #{field}" unless offset

      if type == FFI::TYPE_CHARARR
        (@pointer + offset).read_string
      else
        @pointer.get_at_offset(offset, type)
      end
    end

    def []=(field, val)
      offset, type = @cspec[field]
      raise "Unknown field #{field}" unless offset

      @pointer.set_at_offset(offset, type, val)
      return val
    end

  end
end
