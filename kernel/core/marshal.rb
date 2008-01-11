# depends on: module.rb

class NilClass
  def to_marshal(depth = -1, version = true, subclass = nil)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_NIL
  end
end

class TrueClass
  def to_marshal(depth = -1, version = true, subclass = nil)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_TRUE
  end
end

class FalseClass
  def to_marshal(depth = -1, version = true, subclass = nil)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_FALSE
  end
end

class Class
  def to_marshal(depth = -1, version = true, subclass = nil)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_CLASS +
    Marshal::serialize_integer(self.name.length) + self.name
  end
end

class Module
  def to_marshal(depth = -1, version = true, subclass = nil)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_MODULE +
    Marshal::serialize_integer(self.name.length) + self.name
  end
end

class Symbol
  def to_marshal(depth = -1, version = true, subclass = nil)
    str = self.to_s
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_SYMBOL +
    Marshal::serialize_integer(str.length) + str
  end
end

class String
  def to_marshal(depth = -1, version = true, subclass = nil)
    ivar_prefix = Marshal::serialize_instance_variables_prefix(self)
    ivar_suffix = Marshal::serialize_instance_variables_suffix(self, depth, false, subclass)
    (version ? Marshal::VERSION_STRING : '') + ivar_prefix +
    Marshal::serialize_extended_object(self, depth) +
    Marshal::serialize_user_class(self, depth, false, subclass) +
    Marshal::TYPE_STRING +
    Marshal::serialize_integer(self.length) + self + ivar_suffix
  end
end

class Integer
  def to_marshal(depth = -1, version = true, subclass = nil)
    if self >= -2**30 and self <= (2**30 - 1)
      to_marshal_fixnum(depth, version, subclass)
    else
      to_marshal_bignum(depth, version, subclass)
    end
  end

  def to_marshal_fixnum(depth = -1, version = true, subclass = nil)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::TYPE_FIXNUM +
    Marshal::serialize_integer(self)
  end

  def to_marshal_bignum(depth = -1, version = true, subclass = nil)
    str = (version ? Marshal::VERSION_STRING : '') +
          Marshal::TYPE_BIGNUM +
          (self < 0 ? '-' : '+') + "\0"
    size_index = str.length - 1
    cnt = 0
    num = self.abs
    while num != 0
      str << Marshal::to_byte(num & 0xFF)
      num >>= 8
      cnt += 1
    end
    if cnt % 2 == 1
      str << "\0"
      cnt += 1
    end
    str[size_index] = Marshal::to_byte(((cnt - 4) / 2) + 7)
    str
  end
end

class Regexp
  def to_marshal(depth = -1, version = true, subclass = nil)
    str = self.source
    ivar_prefix = Marshal::serialize_instance_variables_prefix(self)
    ivar_suffix = Marshal::serialize_instance_variables_suffix(self, depth, false, subclass)
    (version ? Marshal::VERSION_STRING : '') + ivar_prefix +
    Marshal::serialize_extended_object(self, depth) +
    Marshal::serialize_user_class(self, depth, false, subclass) +
    Marshal::TYPE_REGEXP +
    Marshal::serialize_integer(str.length) + str +
    Marshal::to_byte(self.options & 0x7) + ivar_suffix
  end
end

class Struct
  def to_marshal(depth = -1, version = true, subclass = nil)
    str = (version ? Marshal::VERSION_STRING : '') +
          Marshal::serialize_extended_object(self, depth) +
          Marshal::TYPE_STRUCT +
          self.class.name.to_sym.to_marshal(depth, false, subclass) +
          Marshal::serialize_integer(self.length)
    self.each_pair do |sym, val|
      str << sym.to_marshal(depth, false, subclass) +
             val.to_marshal(depth, false, subclass)
    end
    str
  end
end

class Object
  def to_marshal(depth = -1, version = true, subclass = nil)
    ivar_suffix = Marshal::serialize_instance_variables_suffix(self, depth, false, subclass)
    (version ? Marshal::VERSION_STRING : '') +
    Marshal::serialize_extended_object(self, depth) +
    Marshal::TYPE_OBJECT +
    self.class.name.to_sym.to_marshal(depth, false, subclass) + ivar_suffix
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
  TYPE_USRMARSHAL = 'U'  # no specs
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

  def self.dump(obj, depth = -1, io = nil)
    serialize(obj, depth)
  end

  def self.serialize(obj, depth)
    cls = obj.class
    sup = get_superclass(cls)
    [cls.to_s, sup.to_s].each do |class_name|
      case class_name
      when 'String'
        if obj.class != String
          return obj.to_marshal(depth, true, obj.class)
        end
      when 'Regexp'
        if obj.class != Regexp
          return obj.to_marshal(depth, true, obj.class)
        end
      end
    end
    obj.to_marshal(depth)
  end

  def self.serialize_integer(n)
    if n == 0
      s = to_byte(n)
    elsif n > 0 and n < 123
      s = to_byte(n + 5)
    elsif n < 0 and n > -124
      s = to_byte(256 + (n - 5))
    else
      s = "\x00"
      cnt = 0
      4.times do
        s << to_byte(n & 0xFF)
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
      TYPE_IVAR
    else
      ''
    end
  end

  def self.serialize_instance_variables_suffix(obj, depth = -1, version = false, subclass = nil)
    if obj.class == Object or obj.instance_variables.length > 0
      str = serialize_integer(obj.instance_variables.length)
      obj.instance_variables.each do |ivar|
        sym = ivar.to_sym
        o = obj.instance_variable_get(sym)
        str << sym.to_marshal(depth, false, subclass) +
               o.to_marshal(depth, false, subclass)
      end
      str
    else
      ''
    end
  end

  def self.serialize_extended_object(obj, depth = -1)
    str = ''
    get_module_names(obj).each do |mod_name|
      str << TYPE_EXTENDED + mod_name.to_sym.to_marshal(depth, false)
    end
    str
  end

  def self.serialize_user_class(obj, depth, version, subclass)
    if obj.class == subclass
      TYPE_UCLASS + obj.class.name.to_sym.to_marshal(depth, false, subclass)
    else
      ''
    end
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
