# -*- encoding: us-ascii -*-

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

  def instance_variables
    # Hide the ivars used to store the struct fields
    attr_syms = _attrs.map { |a| "@#{a}".to_sym }
    super() - Rubinius::Type.convert_to_names(attr_syms)
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

  def each(&block)
    return to_enum :each unless block_given?
    values.each(&block)
    self
  end

  def each_pair
    return to_enum :each_pair unless block_given?
    _attrs.each { |var| yield var, instance_variable_get(:"@#{var}") }
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
    return Rubinius::Type.convert_to_names(self::STRUCT_ATTRS.dup)
  end

  def members
    return self.class.members
  end

  def select(&block)
    to_a.select(&block)
  end

  def to_a
    return _attrs.map { |var| instance_variable_get :"@#{var}" }
  end

  alias_method :values, :to_a

  def values_at(*args)
    to_a.values_at(*args)
  end
end
