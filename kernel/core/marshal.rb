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
    out << Marshal.serialize_instance_variables_suffix(ms, self)
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
    def consumed; @consumed; end
    def symbols; @symbols; end
    def objects; @objects; end
    def modules; @modules; end
    def has_ivar; @has_ivar; end
    def user_class; @user_class; end
    def nested; @nested; end
    def proc; @proc; end
    def call_proc; @call_proc; end

    def depth=(v); @depth = v; end
    def links=(v); @links = v; end
    def symlinks=(v); @symlinks = v; end
    def consumed=(v); @consumed = v; end
    def symbols=(v); @symbols = v; end
    def objects=(v); @objects = v; end
    def modules=(v); @modules = v; end
    def has_ivar=(v); @has_ivar = v; end
    def user_class=(v); @user_class = v; end
    def nested=(v); @nested = v; end
    def proc=(v); @proc = v; end
    def call_proc=(v); @call_proc = v; end

    def initialize
      @depth = -1; @links = {}; @symlinks = {}
      @consumed = 0; @symbols = []; @objects = []
      @modules = []; @has_ivar = false; @nested = false
      @call_proc = false
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

  def self.load(obj, proc = nil)
    ms = State.new
    ms.proc = proc
    ms.call_proc = true if ms.proc
    if obj.respond_to? :to_str
      str = obj.to_s
    elsif obj.respond_to? :read
      str = obj.read
    elsif obj.respond_to? :getc  # FIXME - don't read all of it upfront
      str = ''
      str << c while (c = obj.getc.chr)
    else
      raise TypeError, "instance of IO needed"
    end
    construct(ms, str)
  end

  def self.serialize(ms, obj)
    raise ArgumentError, "exceed depth limit" if ms.depth == 0; ms.depth -= 1

    if obj.respond_to? :_dump
      return serialize_custom_object_ud(ms, obj)
    elsif obj.respond_to? :marshal_dump
      return serialize_custom_object_mud(ms, obj)
    end

    obj.to_marshal(ms)
  end

  def self.construct(ms, str)
    i = ms.consumed
    ms.consumed += 1
    if i == 0 or i == 1
      construct(ms, str)
    else
      c = str[i].chr
      case c
      when TYPE_NIL
        ms.proc.call(nil) if ms.call_proc
        nil
      when TYPE_TRUE
        ms.proc.call(true) if ms.call_proc
        true
      when TYPE_FALSE
        ms.proc.call(false) if ms.call_proc
        false
      when TYPE_CLASS, TYPE_MODULE
        cls_mod = Module.const_get(construct_symbol(ms, str))
        ms.proc.call(cls_mod) if ms.call_proc
        store_unique_object(ms, cls_mod)
      when TYPE_FIXNUM
        obj = construct_integer(ms, str)
        ms.proc.call(obj) if ms.call_proc
        obj
      when TYPE_BIGNUM
        construct_bignum(ms, str)
      when TYPE_FLOAT
        construct_float(ms, str)
      when TYPE_SYMBOL
        construct_symbol(ms, str)
      when TYPE_STRING
        construct_string(ms, str)
      when TYPE_REGEXP
        construct_regexp(ms, str)
      when TYPE_ARRAY
        construct_array(ms, str)
      when TYPE_HASH, TYPE_HASH_DEF
        construct_hash(ms, str, c)
      when TYPE_STRUCT
        construct_struct(ms, str)
      when TYPE_OBJECT
        construct_object(ms, str)
      when TYPE_USERDEF
        construct_custom_object_ul(ms, str)
      when TYPE_USRMARSHAL
        construct_custom_object_mul(ms, str)
      when TYPE_LINK
        num = construct_integer(ms, str)
        ms.objects[num-1]
      when TYPE_SYMLINK
        num = construct_integer(ms, str)
        ms.symbols[num]
      when TYPE_EXTENDED
        ms.call_proc = false
        sym = construct(ms, str)
        ms.modules << sym
        store_unique_object(ms, sym)
        construct(ms, str)
      when TYPE_UCLASS
        ms.call_proc = false
        sym = construct(ms, str)
        ms.user_class = sym
        store_unique_object(ms, sym)
        construct(ms, str)
      when TYPE_IVAR
        ms.has_ivar = true
        construct(ms, str)
      else
        raise ArgumentError, "load error"
      end
    end
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

  def self.serialize_custom_object_ud(ms, obj)
    str = obj._dump(ms.depth)
    raise TypeError, "_dump() must return string" if str.class != String
    out = serialize_instance_variables_prefix(str)
    out << TYPE_USERDEF + serialize_duplicate(ms, obj.class.name.to_sym)
    out << serialize_integer(str.length) + str
    out << serialize_instance_variables_suffix(ms, str)
  end

  def self.serialize_custom_object_mud(ms, obj)
    val = obj.marshal_dump
    out = TYPE_USRMARSHAL + serialize_duplicate(ms, obj.class.name.to_sym)
    out << val.to_marshal(ms)
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

  def self.construct_integer(ms, str)
    i = ms.consumed
    ms.consumed += 1
    n = str[i]
    if (n > 0 and n < 5) or n > 251
      (size, signed) = n > 251 ? [256 - n, 2**((256 - n)*8)] : [n, 0]
      result = 0
      (0...size).each do |exp|
        i += 1
        result += (str[i] * 2**(exp*8))
      end
      ms.consumed += size
      result - signed
    elsif n > 127
      (n - 256) + 5
    elsif n > 4
      n - 5
    else
      n
    end
  end

  def self.construct_bignum(ms, str)
    result = 0
    i = ms.consumed
    ms.consumed += 1
    sign = str[i].chr == '-' ? -1 : 1
    size = construct_integer(ms, str) * 2
    i = ms.consumed
    (0...size).each do |exp|
      result += (str[i] * 2**(exp*8))
      i += 1
    end
    ms.consumed += size
    obj = result * sign
    ms.proc.call(obj) if ms.call_proc
    store_unique_object(ms, obj)
  end

  def self.construct_float(ms, str)
    s = get_byte_sequence(ms, str)
    if s == "nan"
      obj = 0.0 / 0.0
    elsif s == "inf"
      obj = 1.0 / 0.0
    elsif s == "-inf"
      obj = 1.0 / -0.0
    else
      obj = s.to_f
    end
    ms.proc.call(obj) if ms.call_proc
    store_unique_object(ms, obj)
  end

  def self.construct_string(ms, str)
    modules = ms.modules; ms.modules = []
    obj = get_byte_sequence(ms, str)
    obj = get_user_class(ms).new(obj) if ms.user_class
    set_instance_variables(ms, str, obj)
    extend_object(modules, obj)
    ms.proc.call(obj) if ms.call_proc
    store_unique_object(ms, obj)
  end

  def self.construct_symbol(ms, str)
    obj = get_byte_sequence(ms, str).to_sym
    ms.proc.call(obj) if ms.call_proc
    store_unique_object(ms, obj)
  end

  def self.construct_regexp(ms, str)
    modules = ms.modules; ms.modules = []
    s = get_byte_sequence(ms, str)
    i = ms.consumed
    ms.consumed += 1
    if ms.user_class
      obj = get_user_class(ms).new(s, str[i])
    else
      obj = Regexp.new(s, str[i])
    end
    set_instance_variables(ms, str, obj)
    extend_object(modules, obj)
    ms.proc.call(obj) if ms.call_proc
    store_unique_object(ms, obj)
  end

  def self.construct_array(ms, str)
    has_ivar = ms.has_ivar; ms.has_ivar = false
    modules = ms.modules; ms.modules = []
    obj = ms.user_class ? get_user_class(ms).new : []
    store_unique_object(ms, obj) if ms.nested
    ms.nested = true
    ms.call_proc = true if ms.proc
    construct_integer(ms, str).times do
      obj << construct(ms, str)
    end
    ms.has_ivar = has_ivar
    set_instance_variables(ms, str, obj)
    extend_object(modules, obj)
    ms.proc.call(obj) if ms.proc
    obj
  end

  def self.construct_hash(ms, str, type)
    has_ivar = ms.has_ivar; ms.has_ivar = false
    modules = ms.modules; ms.modules = []
    obj = ms.user_class ? get_user_class(ms).new : {}
    store_unique_object(ms, obj) if ms.nested
    ms.nested = true
    ms.call_proc = true if ms.proc
    construct_integer(ms, str).times do
      key = construct(ms, str)
      val = construct(ms, str)
      obj[key] = val
    end
    ms.call_proc = true if ms.proc
    obj.default = construct(ms, str) if type == TYPE_HASH_DEF
    ms.has_ivar = has_ivar
    set_instance_variables(ms, str, obj)
    extend_object(modules, obj)
    ms.proc.call(obj) if ms.proc
    obj
  end

  def self.construct_struct(ms, str)
    modules = ms.modules; ms.modules = []
    symbols = []; values = []
    ms.call_proc = false
    sym = construct(ms, str)
    store_unique_object(ms, sym)
    ms.nested = true
    construct_integer(ms, str).times do
      ms.call_proc = false
      symbols << construct(ms, str)
      ms.call_proc = true if ms.proc
      values << construct(ms, str)
    end
    obj = Struct.new(sym.to_s.sub(/\AStruct\:\:/, ''), *symbols).new
    (0...symbols.length).each do |i|
      obj[symbols[i]] = values[i]
    end
    set_instance_variables(ms, str, obj)
    extend_object(modules, obj)
    ms.proc.call(obj) if ms.proc
    store_unique_object(ms, obj)
  end

  def self.construct_object(ms, str)
    modules = ms.modules; ms.modules = []
    ms.call_proc = false
    obj = Module.const_get(construct(ms, str)).allocate
    store_unique_object(ms, obj) if ms.nested
    ms.has_ivar = true
    set_instance_variables(ms, str, obj)
    extend_object(modules, obj)
    ms.proc.call(obj) if ms.proc
    obj
  end

  def self.construct_custom_object_ul(ms, str)
    sym = construct(ms, str)
    store_unique_object(ms, sym)
    s = get_byte_sequence(ms, str)
    set_instance_variables(ms, str, s)
    Module.const_get(sym)._load(s)
  end

  def self.construct_custom_object_mul(ms, str)
    sym = construct(ms, str)
    store_unique_object(ms, sym)
    obj = Module.const_get(sym).new
    obj.marshal_load(obj)
  end

  def self.set_instance_variables(ms, str, obj)
    if ms.has_ivar
      ms.has_ivar = false
      ms.nested = true
      construct_integer(ms, str).times do
        ms.call_proc = false
        sym = construct(ms, str)
        ms.call_proc = true if ms.proc
        val = construct(ms, str)
        obj.instance_variable_set(prepare_ivar(sym), val)
      end
    end
    obj
  end

  def self.get_byte_sequence(ms, str)
    size = construct_integer(ms, str)
    i = ms.consumed
    k = i + size
    ms.consumed += size
    str[i...k]
  end

  def self.store_unique_object(ms, obj)
    if obj.class == Symbol
      unless ms.symlinks[obj.object_id]
        ms.symlinks[obj.object_id] = true
        ms.symbols << obj
      end
    else
      unless ms.links[obj.object_id]
        ms.links[obj.object_id] = true
        ms.objects << obj
      end
    end
    obj
  end

  def self.extend_object(modules, obj)
    modules.reverse_each do |sym|
      mod = Module.const_get(sym)
      obj.extend(mod)
    end
    obj
  end

  def self.prepare_ivar(sym)
    str = sym.to_s
    str.sub!(/\A\@+/, '')
    ('@' + str).to_sym
  end

  def self.get_user_class(ms)
    cls = Module.const_get(ms.user_class)
    ms.user_class = nil
    cls
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
