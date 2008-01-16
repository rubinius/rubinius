# depends on: module.rb class.rb

class NilClass
  def to_marshal(ms = nil)
    Marshal::TYPE_NIL
  end
end

class TrueClass
  def to_marshal(ms = nil)
    Marshal::TYPE_TRUE
  end
end

class FalseClass
  def to_marshal(ms = nil)
    Marshal::TYPE_FALSE
  end
end

class Class
  def to_marshal(ms = nil)
    raise TypeError, "can't dump anonymous class #{self}" if self.name == ''
    Marshal::TYPE_CLASS +
    Marshal.serialize_integer(self.name.length) + self.name
  end
end

class Module
  def to_marshal(ms = nil)
    raise TypeError, "can't dump anonymous module #{self}" if self.name == ''
    Marshal::TYPE_MODULE +
    Marshal.serialize_integer(self.name.length) + self.name
  end
end

class Symbol
  def to_marshal(ms = nil)
    str = self.to_s
    Marshal::TYPE_SYMBOL +
    Marshal.serialize_integer(str.length) + str
  end
end

class String
  def to_marshal(ms = Marshal::State.new)
    out = Marshal.serialize_instance_variables_prefix(self)
    out << Marshal.serialize_extended_object(ms, self)
    out << Marshal.serialize_user_class(ms, self, String)
    out << Marshal::TYPE_STRING
    out << Marshal.serialize_integer(self.length) + self
    out << Marshal.serialize_instance_variables_suffix(ms, self)
  end
end

class Integer
  def to_marshal(ms = nil)
    if Marshal.fixnum? self
      to_marshal_fixnum
    else
      to_marshal_bignum
    end
  end

  def to_marshal_fixnum
    Marshal::TYPE_FIXNUM +
    Marshal.serialize_integer(self)
  end

  def to_marshal_bignum
    str = Marshal::TYPE_BIGNUM + (self < 0 ? '-' : '+')
    cnt = 0
    num = self.abs
    while num != 0
      str << Marshal.to_byte(num)
      num >>= 8
      cnt += 1
    end
    if cnt % 2 == 1
      str << "\0"
      cnt += 1
    end
    str[0..1] + Marshal.serialize_integer(cnt / 2) + str[2..-1]
  end
end

class Regexp
  def to_marshal(ms = Marshal::State.new)
    str = self.source
    out = Marshal.serialize_instance_variables_prefix(self)
    out << Marshal.serialize_extended_object(ms, self)
    out << Marshal.serialize_user_class(ms, self, Regexp)
    out << Marshal::TYPE_REGEXP
    out << Marshal.serialize_integer(str.length) + str
    out << Marshal.to_byte(self.options & 0x7)
    out << Marshal.serialize_instance_variables_suffix(ms, self)
  end
end

class Struct
  def to_marshal(ms = Marshal::State.new)
    out = Marshal.serialize_extended_object(ms, self)
    out << Marshal::TYPE_STRUCT
    out << Marshal.serialize_duplicate(ms, self.class.name.to_sym)
    out << Marshal.serialize_integer(self.length)
    self.each_pair do |sym, val|
      out << Marshal.serialize_duplicate(ms, sym)
      out << Marshal.serialize_duplicate(ms, val)
    end
    out
  end
end

class Array
  def to_marshal(ms = Marshal::State.new)
    out = Marshal.serialize_instance_variables_prefix(self)
    out << Marshal.serialize_extended_object(ms, self)
    out << Marshal.serialize_user_class(ms, self, Array)
    out << Marshal::TYPE_ARRAY
    out << Marshal.serialize_integer(self.length)
    unless self.empty?
      raise ArgumentError, "exceed depth limit" if ms.depth == 0; ms.depth -= 1
      self.each do |element|
        out << Marshal.serialize_duplicate(ms, element)
      end
      ms.depth += 1
    end
    out + Marshal.serialize_instance_variables_suffix(ms, self)
  end
end

class Hash
  def to_marshal(ms = Marshal::State.new)
    raise TypeError, "can't dump hash with default proc" if self.default_proc
    out = Marshal.serialize_instance_variables_prefix(self)
    out << Marshal.serialize_extended_object(ms, self)
    out << Marshal.serialize_user_class(ms, self, Hash)
    out << (self.default ? Marshal::TYPE_HASH_DEF : Marshal::TYPE_HASH)
    out << Marshal.serialize_integer(self.length)
    unless self.empty?
      raise ArgumentError, "exceed depth limit" if ms.depth == 0; ms.depth -= 1
      self.each_pair do |(key, val)|
        out << Marshal.serialize_duplicate(ms, key)
        out << Marshal.serialize_duplicate(ms, val)
      end
      ms.depth += 1
    end
    out << (self.default ? Marshal.serialize_duplicate(ms, self.default) : '')
    out << Marshal.serialize_instance_variables_suffix(ms, self)
  end
end

class Float
  def to_marshal(ms = nil)
    str = if self.nan?
            "nan"
          elsif self.zero?
            (1.0 / self) < 0 ? '-0' : '0'
          elsif self.infinite?
            self < 0 ? "-inf" : "inf"
          else
            "%.*g" % [17, self] + Marshal.serialize_float_thing(self)
          end
    Marshal::TYPE_FLOAT +
    Marshal.serialize_integer(str.length) + str
  end
end

class Object
  def to_marshal(ms = Marshal::State.new)
    out = Marshal.serialize_extended_object(ms, self)
    out << Marshal::TYPE_OBJECT
    out << Marshal.serialize_duplicate(ms, self.class.name.to_sym)
    out << Marshal.serialize_instance_variables_suffix(ms, self, true)
  end
end

module Marshal

  MAJOR_VERSION = 4
  MINOR_VERSION = 8

  VERSION_STRING = "\x04\x08"

  TYPE_NIL = '0'
  TYPE_TRUE = 'T'
  TYPE_FALSE = 'F'
  TYPE_FIXNUM = 'i'

  TYPE_EXTENDED = 'e'
  TYPE_UCLASS = 'C'
  TYPE_OBJECT = 'o'
  TYPE_DATA = 'd'  # no specs
  TYPE_USERDEF = 'u'
  TYPE_USRMARSHAL = 'U'
  TYPE_FLOAT = 'f'
  TYPE_BIGNUM = 'l'
  TYPE_STRING = '"'
  TYPE_REGEXP = '/'
  TYPE_ARRAY = '['
  TYPE_HASH = '{'
  TYPE_HASH_DEF = '}'
  TYPE_STRUCT = 'S'
  TYPE_MODULE_OLD = 'M'  # no specs
  TYPE_CLASS = 'c'
  TYPE_MODULE = 'm'

  TYPE_SYMBOL = ':'
  TYPE_SYMLINK = ';'

  TYPE_IVAR = 'I'
  TYPE_LINK = '@'

  class State
    def depth; @depth; end
    def links; @links; end
    def symlinks; @symlinks; end

    def depth=(v); @depth = v; end
    def links=(v); @links = v; end
    def symlinks=(v); @symlinks = v; end

    def initialize
      @depth = -1; @links = {}; @symlinks = {}
    end
  end

  def self.dump(obj, *args)
    ms = State.new
    if args.length == 2
      if args[1] and not args[1].kind_of? Integer
        raise TypeError, "can't convert #{args[1].class} into Integer"
      elsif not args[0].respond_to? :write
        raise TypeError, "instance of IO needed"
      end
      ms.depth = args[1] == nil ? -1 : args[1]
      args[0].write(VERSION_STRING + serialize(ms, obj))
      args[0]
    elsif args.length == 1
      if args[0].kind_of? Integer
        ms.depth = args[0]
        VERSION_STRING + serialize(ms, obj)
      elsif args[0].respond_to? :write
        args[0].write(VERSION_STRING + serialize(ms, obj))
        args[0]
      else
        raise TypeError, "instance of IO needed"
      end
    else
      VERSION_STRING + serialize(ms, obj)
    end
  end

  def self.serialize(ms, obj)
    raise ArgumentError, "exceed depth limit" if ms.depth == 0; ms.depth -= 1

    if obj.respond_to? :_dump
      return serialize_custom_object_AA(ms, obj)
    elsif obj.respond_to? :marshal_dump
      return serialize_custom_object_BB(ms, obj)
    end

    obj.to_marshal(ms)
  end

  def self.serialize_integer(n)
    if n == 0
      s = to_byte(n)
    elsif n > 0 and n < 123
      s = to_byte(n + 5)
    elsif n < 0 and n > -124
      s = to_byte(256 + (n - 5))
    else
      s = "\0"
      cnt = 0
      4.times do
        s << to_byte(n)
        n >>= 8
        cnt += 1
        break if n == 0 or n == -1
      end
      s[0] = to_byte(n < 0 ? 256 - cnt : cnt)
    end
    s
  end

  def self.serialize_instance_variables_prefix(obj)
    if obj.instance_variables.length > 0
      TYPE_IVAR + ''
    else
      ''
    end
  end

  def self.serialize_instance_variables_suffix(ms, obj, force = false)
    if force or obj.instance_variables.length > 0
      str = serialize_integer(obj.instance_variables.length)
      obj.instance_variables.each do |ivar|
        sym = ivar.to_sym
        val = obj.instance_variable_get(sym)
        str << serialize_duplicate(ms, sym)
        str << serialize_duplicate(ms, val)
      end
      str
    else
      ''
    end
  end

  def self.serialize_extended_object(ms, obj)
    str = ''
    get_module_names(obj).each do |mod_name|
      str << TYPE_EXTENDED + serialize_duplicate(ms, mod_name.to_sym)
    end
    str
  end

  def self.serialize_user_class(ms, obj, cls)
    if obj.class != cls
      TYPE_UCLASS + serialize_duplicate(ms, obj.class.name.to_sym)
    else
      ''
    end
  end

  def self.serialize_custom_object_AA(ms, obj)
    str = obj._dump(ms.depth)
    raise TypeError, "_dump() must return string" if str.class != String
    out = serialize_instance_variables_prefix(str)
    out << TYPE_USERDEF + obj.class.name.to_sym.to_marshal
    out << serialize_integer(str.length) + str
    out << serialize_instance_variables_suffix(ms, str)
  end

  def self.serialize_custom_object_BB(ms, obj)
    val = obj.marshal_dump
    TYPE_USRMARSHAL + obj.class.name.to_sym.to_marshal +
    val.to_marshal(ms)
  end

  def self.serialize_duplicate(ms, obj)
    if obj.class == Symbol
      dup_id = ms.symlinks[obj.object_id]
      if dup_id
        str = TYPE_SYMLINK + serialize_integer(dup_id)
      else
        ms.symlinks[obj.object_id] = ms.symlinks.length
        str = obj.to_marshal
      end
    else
      dup_id = ms.links[obj.object_id]
      if dup_id
        str = TYPE_LINK + serialize_integer(dup_id)
      else
        if linkable_duplicate? obj
          ms.links[obj.object_id] = ms.links.length.succ
        end
        str = obj.to_marshal(ms)
      end
    end
    str
  end

  def self.linkable_duplicate?(obj)
    if fixnum?(obj) or [NilClass, TrueClass, FalseClass].include? obj.class
      false
    else
      true
    end
  end

  def self.fixnum?(n)
    if n.kind_of?(Integer) and n >= -2**30 and n <= (2**30 - 1)
      true
    else
      false
    end
  end

  def self.serialize_float_thing(flt)
    str = ''
    (flt, ) = modf(ldexp(frexp(flt.abs), 37));
    str << "\0" if flt > 0
    while flt > 0
      (flt, n) = modf(ldexp(flt, 32))
      n = n.to_i
      str << to_byte(n >> 24)
      str << to_byte(n >> 16)
      str << to_byte(n >> 8)
      str << to_byte(n)
    end
    str.chomp!("\0") while str[-1] == 0
    str
  end

  def self.frexp(flt)
    p = MemoryPointer.new(:int)
    flt = Platform::Float.frexp(flt, p)
    p.free
    flt
  end

  def self.modf(flt)
    p = MemoryPointer.new(:double)
    flt = Platform::Float.modf(flt, p)
    num = p.read_float
    p.free
    [flt, num]
  end

  def self.ldexp(flt, exp)
    Platform::Float.ldexp(flt, exp)
  end

  def self.get_superclass(cls)
    sup = cls.superclass
    while sup and sup.superclass and sup.superclass != Object
      sup = sup.superclass
    end
    sup
  end

  def self.get_module_names(obj)
    names = []
    sup = obj.metaclass.superclass
    while sup and [Module, IncludedModule].include? sup.class
      names << sup.name
      sup = sup.superclass
    end
    names
  end

  def self.to_byte(n)
    [n].pack('C')
  end
end
