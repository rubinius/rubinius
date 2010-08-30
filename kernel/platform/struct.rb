module FFI
  ##
  # Represents a C struct as ruby class.

  class Struct

    attr_reader :pointer

    def self.layout(*spec)
      return @layout if spec.size == 0

      # Pick up a enclosing FFI::Library
      ss = Rubinius::StaticScope.of_sender
      if par = ss.parent and par.module.kind_of?(FFI::Library)
        @enclosing_module = par.module
      else
        @enclosing_module = nil
      end

      cspec = Rubinius::LookupTable.new
      i = 0

      @size = 0
      @members = []

      while i < spec.size
        name = spec[i]
        @members << name

        f = spec[i + 1]

        if @enclosing_module
          code = @enclosing_module.find_type(f)
        end

        code ||= FFI.find_type(f)
        type_size = FFI.type_size(code)

        offset = spec[i + 2]

        if offset.kind_of?(Fixnum)
          i += 3
        else
          offset = @size

          mod = offset % type_size
          unless mod == 0
            # we need to align it.
            offset += (type_size - mod)
          end

          i += 2
        end

        cspec[name] = [offset, code]
        ending = offset + type_size
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

    def values
      members.map { |m| self[m] }
    end

  end
end
