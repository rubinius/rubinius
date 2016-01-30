module Rubinius
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
        if idx = str.index(0)
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
            type_code = @enclosing_module.find_type(f)
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
end
end
class Struct
  include Enumerable

  class << self
    alias_method :subclass_new, :new
  end

  def self.new(klass_name, *attrs, &block)
    if klass_name
      begin
        klass_name = StringValue klass_name
      rescue TypeError
        attrs.unshift klass_name
        klass_name = nil
      end
    end

    attrs = attrs.map do |a|
      case a
      when Symbol
        a
      when String
        sym = a.to_sym
        unless sym.kind_of? Symbol
          raise TypeError, "#to_sym didn't return a symbol"
        end
        sym
      else
        raise TypeError, "#{a.inspect} is not a symbol"
      end
    end

    klass = Class.new self do
      _specialize attrs
      attr_accessor(*attrs)

      def self.new(*args, &block)
        return subclass_new(*args, &block)
      end

      def self.[](*args)
        return new(*args)
      end

      const_set :STRUCT_ATTRS, attrs
    end

    Struct.const_set klass_name, klass if klass_name

    klass.module_eval(&block) if block

    return klass
  end

  # Don't specialize any thing created in the kernel. We hook up
  # better form of this in delta.
  def self._specialize(attrs)
  end

  def self.make_struct(name, attrs)
    new name, *attrs
  end

  def _attrs # :nodoc:
    return self.class::STRUCT_ATTRS
  end
  private :_attrs

  def select
    return to_enum(:select) { size } unless block_given?

    to_a.select do |v|
      yield v
    end
  end

  def to_h
    Hash[each_pair.to_a]
  end

  def to_s
    return "[...]" if Thread.guarding? self

    Thread.recursion_guard self do
      values = []

      _attrs.each do |var|
        val = instance_variable_get :"@#{var}"
        values << "#{var}=#{val.inspect}"
      end

      name = self.class.name

      if name.nil? || name.empty?
        "#<struct #{values.join(', ')}>"
      else
        "#<struct #{self.class.name} #{values.join(', ')}>"
      end
    end
  end

  alias_method :inspect, :to_s

  def instance_variables
    # Hide the ivars used to store the struct fields
    super() - _attrs.map { |a| "@#{a}".to_sym }
  end

  def initialize(*args)
    attrs = _attrs

    unless args.length <= attrs.length
      raise ArgumentError, "Expected #{attrs.size}, got #{args.size}"
    end

    attrs.each_with_index do |attr, i|
      instance_variable_set :"@#{attr}", args[i]
    end
  end

  private :initialize

  def ==(other)
    return false if self.class != other.class

    Thread.detect_recursion self, other do
      return self.values == other.values
    end

    # Subtle: if we are here, we are recursing and haven't found any difference, so:
    true
  end

  def [](var)
    case var
    when Symbol, String
      # ok
    else
      var = Integer(var)
      a_len = _attrs.length
      if var > a_len - 1
        raise IndexError, "offset #{var} too large for struct(size:#{a_len})"
      end
      if var < -a_len
        raise IndexError, "offset #{var + a_len} too small for struct(size:#{a_len})"
      end
      var = _attrs[var]
    end

    unless _attrs.include? var.to_sym
      raise NameError, "no member '#{var}' in struct"
    end

    return instance_variable_get(:"@#{var}")
  end

  def []=(var, obj)
    case var
    when Symbol
      unless _attrs.include? var
        raise NameError, "no member '#{var}' in struct"
      end
    when String
      var = var.to_sym
      unless _attrs.include? var
        raise NameError, "no member '#{var}' in struct"
      end
    else
      var = Integer(var)
      a_len = _attrs.length
      if var > a_len - 1
        raise IndexError, "offset #{var} too large for struct(size:#{a_len})"
      end
      if var < -a_len
        raise IndexError, "offset #{var + a_len} too small for struct(size:#{a_len})"
      end

      var = _attrs[var]
    end

    return instance_variable_set(:"@#{var}", obj)
  end

  def dig(key, *remaining_keys)
    item = begin
             self[key]
           rescue NameError
             nil
           end
    return item if remaining_keys.empty? || item.nil?

    raise TypeError, "#{item.class} does not have #dig method" unless item.respond_to?(:dig)

    item.dig(*remaining_keys)
  end

  def eql?(other)
    return true if equal? other
    return false if self.class != other.class

    Thread.detect_recursion self, other do
      _attrs.each do |var|
        mine =   instance_variable_get(:"@#{var}")
        theirs = other.instance_variable_get(:"@#{var}")

        return false unless mine.eql? theirs
      end
    end

    # Subtle: if we are here, then no difference was found, or we are recursing
    # In either case, return
    true
  end

  def each
    return to_enum(:each) { size } unless block_given?
    values.each do |v|
      yield v
    end
    self
  end

  def each_pair
    return to_enum(:each_pair) { size } unless block_given?
    _attrs.each { |var| yield [var, instance_variable_get(:"@#{var}")] }
    self
  end

  def hash
    hash_val = size
    return _attrs.size if Thread.detect_outermost_recursion self do
      _attrs.each { |var| hash_val ^= instance_variable_get(:"@#{var}").hash }
    end
    return hash_val
  end

  def length
    return _attrs.length
  end

  alias_method :size, :length

  def self.length
    return self::STRUCT_ATTRS.size
  end

  def self.members
    self::STRUCT_ATTRS.dup
  end

  def members
    return self.class.members
  end

  def to_a
    return _attrs.map { |var| instance_variable_get :"@#{var}" }
  end

  alias_method :values, :to_a

  def values_at(*args)
    to_a.values_at(*args)
  end
end
class Struct
  Struct.new 'Tms', :utime, :stime, :cutime, :cstime, :tutime, :tstime

  class Tms
    def initialize(utime=nil, stime=nil, cutime=nil, cstime=nil,
                   tutime=nil, tstime=nil)
      @utime = utime
      @stime = stime
      @cutime = cutime
      @cstime = cstime
      @tutime = tutime
      @tstime = tstime
    end
  end

  def self._specialize(attrs)
    # Because people are crazy, they subclass Struct directly, ie.
    #  class Craptastic < Struct
    #
    # NOT
    #
    #  class Fine < Struct.new(:x, :y)
    #
    # When they do this craptastic act, they'll sometimes define their
    # own #initialize and super into Struct#initialize.
    #
    # When they do this and then do Craptastic.new(:x, :y), this code
    # will accidentally shadow their #initialize. So for now, only run
    # the specialize if we're trying new Struct's directly from Struct itself,
    # not a craptastic Struct subclass.

    return unless superclass.equal? Struct

    # To allow for optimization, we generate code with normal ivar
    # references for all attributes whose names can be written as
    # tIVAR tokens. For example, of the following struct attributes
    #
    #   Struct.new(:a, :@b, :c?, :'d-e')
    #
    # only the first, :a, can be written as a valid tIVAR token:
    #
    #   * :a can be written as @a
    #   * :@b becomes @@b and would be interpreted as a tCVAR
    #   * :c? becomes @c? and be interpreted as the beginning of
    #     a ternary expression
    #   * :'d-e' becomes @d-e and would be interpreted as a method
    #     invocation
    #
    # Attribute names that cannot be written as tIVAR tokens will
    # fall back to using #instance_variable_(get|set).

    args, assigns, hashes, vars = [], [], [], []

    attrs.each_with_index do |name, i|
      name = "@#{name}"

      if name =~ /^@[a-z_]\w*$/i
        assigns << "#{name} = a#{i}"
        vars    << name
      else
        assigns << "instance_variable_set(:#{name.inspect}, a#{i})"
        vars    << "instance_variable_get(:#{name.inspect})"
      end

      args   << "a#{i} = nil"
      hashes << "#{vars[-1]}.hash"
    end

    code = <<-CODE
      def initialize(#{args.join(", ")})
        #{assigns.join(';')}
        self
      end

      def hash
        hash = #{hashes.size}

        return hash if Thread.detect_outermost_recursion(self) do
          hash = hash ^ #{hashes.join(' ^ ')}
        end

        hash
      end

      def to_a
        [#{vars.join(', ')}]
      end

      def length
        #{vars.size}
      end
    CODE

    begin
      mod = Module.new do
        module_eval code
      end
      include mod
    rescue SyntaxError
      # SyntaxError means that something is wrong with the
      # specialization code. Just eat the error and don't specialize.
    end
  end
end
