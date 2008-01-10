# depends on: module.rb

module Marshal

  MAJOR_VERSION = 4
  MINOR_VERSION = 8

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
    cls = obj.class
    sup = get_superclass(cls)

    str = [MAJOR_VERSION, MINOR_VERSION].pack('CC')
    str << serialize(obj, cls, sup, depth)
    str
  end

  def self.serialize(obj, cls, sup, depth)
    [cls.to_s, sup.to_s].each do |class_name|
      case class_name
      when 'NilClass'
        return serialize_nil(obj)
      when 'TrueClass'
        return serialize_true(obj)
      when 'FalseClass'
        return serialize_false(obj)
      when 'Class'
        return serialize_class(obj)
      when 'Module'
        return serialize_module(obj)
      when 'Symbol'
        return serialize_symbol(obj)
      when 'String'
        ivar_prefix = serialize_instance_variables_prefix(obj)
        ivar_suffix = serialize_instance_variables_suffix(obj, cls, sup, depth)
        return ivar_prefix + serialize_string(obj) + ivar_suffix
      when / Integer | Fixnum | Bignum /x
        if obj >= -2**30 and obj <= (2**30 - 1)
          return serialize_fixnum(obj)
        else
          return serialize_bignum(obj)
        end
      when 'Regexp'
        ivar_prefix = serialize_instance_variables_prefix(obj)
        ivar_suffix = serialize_instance_variables_suffix(obj, cls, sup, depth)
        return ivar_prefix + serialize_regexp(obj) + ivar_suffix
      when 'Struct'
        return serialize_struct(obj, depth)
      end
    end
    ivar_suffix = serialize_instance_variables_suffix(obj, cls, sup, depth)
    serialize_object(obj) + ivar_suffix
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

  def self.serialize_nil(obj)
    TYPE_NIL
  end

  def self.serialize_true(obj)
    TYPE_TRUE
  end

  def self.serialize_false(obj)
    TYPE_FALSE
  end

  def self.serialize_symbol(obj)
    str = obj.to_s
    TYPE_SYMBOL + serialize_integer(str.length) + str
  end

  def self.serialize_class(obj)
    TYPE_CLASS + serialize_integer(obj.name.length) + obj.name
  end

  def self.serialize_module(obj)
    TYPE_MODULE + serialize_integer(obj.name.length) + obj.name
  end

  def self.serialize_instance_variables_prefix(obj)
    if obj.instance_variables.length > 0
      TYPE_IVAR
    else
      ''
    end
  end

  def self.serialize_instance_variables_suffix(obj, cls, sup, depth)
    if cls == Object or obj.instance_variables.length > 0
      str = serialize_integer(obj.instance_variables.length)
      obj.instance_variables.each do |ivar|
        o = obj.instance_variable_get(ivar.to_sym)
        str << serialize_symbol(ivar) +
               serialize(o, o.class, get_superclass(o.class), depth)
      end
      str
    else
      ''
    end
  end

  def self.serialize_extended_object(obj)
    str = ''
    get_module_names(obj).each do |mod_name|
      str << TYPE_EXTENDED + serialize_symbol(mod_name)
    end
    str
  end

  def self.serialize_user_class(obj, cls)
    if obj.class != cls
      TYPE_UCLASS + serialize_symbol(obj.class.name)
    else
      ''
    end
  end

  def self.serialize_string(obj)
    serialize_extended_object(obj) +
    serialize_user_class(obj, String) +
    TYPE_STRING + serialize_integer(obj.length) + obj
  end

  def self.serialize_fixnum(obj)
    TYPE_FIXNUM + serialize_integer(obj)
  end

  def self.serialize_bignum(obj)
    str = TYPE_BIGNUM + (obj < 0 ? '-' : '+') + "\0"
    obj = obj.abs
    while obj != 0
      str << to_byte(obj & 0xFF)
      obj >>= 8
    end
    str << (str.length % 2 == 0 ? "\0" : '')
    str[2] = to_byte(((str.length - 7) / 2) + 7)
    str
  end

  def self.serialize_regexp(obj)
    str = obj.source
    serialize_extended_object(obj) +
    serialize_user_class(obj, Regexp) +
    TYPE_REGEXP + serialize_integer(str.length) +
    str + to_byte(obj.options & 0x7)
  end

  def self.serialize_object(obj)
    serialize_extended_object(obj) +
    TYPE_OBJECT + serialize_symbol(obj.class.name)
  end

  def self.serialize_struct(obj, depth)
    str = serialize_extended_object(obj) +
    TYPE_STRUCT + serialize_symbol(obj.class.name) +
    serialize_integer(obj.length)

    obj.each_pair do |sym, val|
      str << serialize_symbol(sym) +
             serialize(val, val.class, get_superclass(val.class), depth)
    end
    str
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
