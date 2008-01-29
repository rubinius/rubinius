# depends on: module.rb class.rb

class NilClass
  def to_marshal(ms)
    Marshal::TYPE_NIL
  end
end

class TrueClass
  def to_marshal(ms)
    Marshal::TYPE_TRUE
  end
end

class FalseClass
  def to_marshal(ms)
    Marshal::TYPE_FALSE
  end
end

class Class
  def to_marshal(ms)
    raise TypeError, "can't dump anonymous class #{self}" if self.name == ''
    Marshal::TYPE_CLASS + ms.serialize_integer(name.length) + name
  end
end

class Module
  def to_marshal(ms)
    raise TypeError, "can't dump anonymous module #{self}" if self.name == ''
    Marshal::TYPE_MODULE + ms.serialize_integer(name.length) + name
  end
end

class Symbol
  def to_marshal(ms)
    if idx = ms.find_symlink(self) then
      Marshal::TYPE_SYMLINK + ms.serialize_integer(idx)
    else
      ms.add_symlink self

      str = to_s
      Marshal::TYPE_SYMBOL + ms.serialize_integer(str.length) + str
    end
  end
end

class String
  def to_marshal(ms)
    out = ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, String)
    out << Marshal::TYPE_STRING
    out << ms.serialize_integer(self.length) << self
    out << ms.serialize_instance_variables_suffix(self)
  end
end

class Fixnum
  def to_marshal(ms)
    Marshal::TYPE_FIXNUM + ms.serialize_integer(self)
  end
end

class Bignum
  def to_marshal(ms)
    str = Marshal::TYPE_BIGNUM + (self < 0 ? '-' : '+')
    cnt = 0
    num = self.abs

    while num != 0
      str << ms.to_byte(num)
      num >>= 8
      cnt += 1
    end

    if cnt % 2 == 1
      str << "\0"
      cnt += 1
    end

    str[0..1] + ms.serialize_integer(cnt / 2) + str[2..-1]
  end
end

class Regexp
  def to_marshal(ms)
    str = self.source
    out = ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Regexp)
    out << Marshal::TYPE_REGEXP
    out << ms.serialize_integer(str.length) + str
    out << ms.to_byte(options & 0x7)
    out << ms.serialize_instance_variables_suffix(self)
  end
end

class Struct
  def to_marshal(ms)
    out =  ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << Marshal::TYPE_STRUCT
    out << ms.serialize(self.class.name.to_sym)
    out << ms.serialize_integer(self.length)
    self.each_pair do |sym, val|
      out << ms.serialize(sym)
      out << ms.serialize(val)
    end
    out << ms.serialize_instance_variables_suffix(self)
    out
  end
end

class Array
  def to_marshal(ms)
    out = ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Array)
    out << Marshal::TYPE_ARRAY
    out << ms.serialize_integer(self.length)
    unless empty? then
      each do |element|
        out << ms.serialize(element)
      end
    end
    out << ms.serialize_instance_variables_suffix(self)
  end
end

class Hash
  def to_marshal(ms)
    raise TypeError, "can't dump hash with default proc" if default_proc
    out = ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Hash)
    out << (self.default ? Marshal::TYPE_HASH_DEF : Marshal::TYPE_HASH)
    out << ms.serialize_integer(length)
    unless empty? then
      each_pair do |(key, val)|
        out << ms.serialize(key)
        out << ms.serialize(val)
      end
    end
    out << (default ? ms.serialize(default) : '')
    out << ms.serialize_instance_variables_suffix(self)
  end
end

class Float
  def to_marshal(ms)
    str = if nan? then
            "nan"
          elsif zero? then
            (1.0 / self) < 0 ? '-0' : '0'
          elsif infinite? then
            self < 0 ? "-inf" : "inf"
          else
            "%.*g" % [17, self] + ms.serialize_float_thing(self)
          end
    Marshal::TYPE_FLOAT + ms.serialize_integer(str.length) + str
  end
end

class Object
  def to_marshal(ms)
    out = ms.serialize_extended_object self
    out << Marshal::TYPE_OBJECT
    out << ms.serialize(self.class.name.to_sym)
    out << ms.serialize_instance_variables_suffix(self, true)
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

    attr_accessor :depth
    attr_accessor :consumed
    attr_accessor :modules
    attr_accessor :has_ivar
    attr_accessor :user_class
    attr_accessor :nested
    attr_accessor :proc
    attr_accessor :call_proc
    attr_accessor :links
    attr_accessor :symlinks
    attr_accessor :objects
    attr_accessor :symbols

    def initialize
      @depth = -1
      @links = {}
      @symlinks = {}
      @consumed = 0
      @symbols = []
      @objects = []
      @modules = []
      @has_ivar = false
      @nested = false
      @proc = nil
      @call_proc = false
    end

    def add_object(obj)
      return if obj.kind_of?(ImmediateValue)
      sz = @links.size
      @objects[sz] = obj
      @links[obj.object_id] = sz
    end

    def add_symlink(obj)
      sz = @symlinks.size
      @symbols[sz] = obj
      @symlinks[obj.object_id] = sz
    end

    def call(obj)
      @proc.call(obj) if @proc and @call_proc
    end

    def construct(str)
      i = @consumed
      @consumed += 1

      if i == 0 or i == 1
        construct str
      else
        c = str[i].chr
        case c
        when TYPE_NIL
          @proc.call nil if @call_proc
          nil
        when TYPE_TRUE
          @proc.call true if @call_proc
          true
        when TYPE_FALSE
          @proc.call false if @call_proc
          false
        when TYPE_CLASS, TYPE_MODULE
          cls_mod = Module.const_lookup construct_symbol(str)
          @proc.call cls_mod if @call_proc
          store_unique_object cls_mod
        when TYPE_FIXNUM
          obj = construct_integer str
          @proc.call obj if @call_proc
          obj
        when TYPE_BIGNUM
          construct_bignum str
        when TYPE_FLOAT
          construct_float str
        when TYPE_SYMBOL
          construct_symbol str
        when TYPE_STRING
          construct_string str
        when TYPE_REGEXP
          construct_regexp str
        when TYPE_ARRAY
          construct_array str
        when TYPE_HASH, TYPE_HASH_DEF
          construct_hash str, c
        when TYPE_STRUCT
          construct_struct str
        when TYPE_OBJECT
          construct_object str
        when TYPE_USERDEF
          construct_custom_object_ul str
        when TYPE_USRMARSHAL
          construct_custom_object_mul str
        when TYPE_LINK
          num = construct_integer str
          @objects[num-1]
        when TYPE_SYMLINK
          num = construct_integer str
          @symbols[num]
        when TYPE_EXTENDED
          @call_proc = false
          sym = construct str
          @modules << sym
          store_unique_object sym
          construct str
        when TYPE_UCLASS
          @call_proc = false
          sym = construct str
          @user_class = sym
          store_unique_object sym
          construct str
        when TYPE_IVAR
          @has_ivar = true
          construct str
        else
          raise ArgumentError, "load error"
        end
      end
    end

    def construct_array(str)
      has_ivar = @has_ivar; @has_ivar = false
      modules = @modules; @modules = []
      obj = @user_class ? get_user_class.new : []
      store_unique_object obj if @nested
      @nested = true
      @call_proc = true if @proc
      construct_integer(str).times do
        obj << construct(str)
      end
      @has_ivar = has_ivar
      set_instance_variables str, obj
      extend_object(modules, obj)
      @proc.call(obj) if @proc
      obj
    end

    def construct_bignum(str)
      result = 0
      i = @consumed
      @consumed += 1
      sign = str[i].chr == '-' ? -1 : 1
      size = construct_integer(str) * 2
      i = @consumed
      (0...size).each do |exp|
        result += (str[i] * 2**(exp*8))
        i += 1
      end
      @consumed += size
      obj = result * sign
      @proc.call(obj) if @call_proc
      store_unique_object obj
    end

    def construct_custom_object_mul(str)
      sym = construct str
      store_unique_object sym

      obj = Module.const_lookup(sym).allocate

      data = construct str
      obj.marshal_load data

      obj
    end

    def construct_custom_object_ul(str)
      sym = construct str
      store_unique_object sym
      s = get_byte_sequence str
      set_instance_variables str, s
      Module.const_lookup(sym)._load(s)
    end

    def construct_float(str)
      s = get_byte_sequence str
      if s == "nan"
        obj = 0.0 / 0.0
      elsif s == "inf"
        obj = 1.0 / 0.0
      elsif s == "-inf"
        obj = 1.0 / -0.0
      else
        obj = s.to_f
      end
      @proc.call obj if @call_proc
      store_unique_object obj
    end

    def construct_hash(str, type)
      has_ivar = @has_ivar; @has_ivar = false
      modules = @modules; @modules = []
      obj = @user_class ? get_user_class.new : {}
      store_unique_object obj if @nested
      @nested = true
      @call_proc = true if @proc
      construct_integer(str).times do
        key = construct str
        val = construct str
        obj[key] = val
      end
      @call_proc = true if @proc
      obj.default = construct str if type == TYPE_HASH_DEF
      @has_ivar = has_ivar
      set_instance_variables str, obj
      extend_object(modules, obj)
      @proc.call(obj) if @proc
      obj
    end

    def construct_integer(str)
      i = @consumed
      @consumed += 1
      n = str[i]
      if (n > 0 and n < 5) or n > 251
        (size, signed) = n > 251 ? [256 - n, 2**((256 - n)*8)] : [n, 0]
        result = 0
        (0...size).each do |exp|
          i += 1
          result += (str[i] * 2**(exp*8))
        end
        @consumed += size
        result - signed
      elsif n > 127
        (n - 256) + 5
      elsif n > 4
        n - 5
      else
        n
      end
    end

    def construct_object(str)
      modules = @modules
      @modules = []
      @call_proc = false

      name = construct str
      mod = Object.const_lookup name
      obj = mod.allocate

      store_unique_object obj if @nested
      @has_ivar = true
      set_instance_variables str, obj
      extend_object(modules, obj)
      @proc.call obj if @proc
      obj
    end

    def construct_regexp(str)
      modules = @modules; @modules = []
      s = get_byte_sequence str
      i = @consumed
      @consumed += 1
      if @user_class
        obj = get_user_class.new(s, str[i])
      else
        obj = Regexp.new(s, str[i])
      end
      set_instance_variables str, obj
      extend_object(modules, obj)
      @proc.call obj if @call_proc
      store_unique_object obj
    end

    def construct_string(str)
      modules = @modules; @modules = []
      obj = get_byte_sequence str
      obj = get_user_class.new obj if @user_class
      set_instance_variables str, obj
      extend_object(modules, obj)
      @proc.call obj if @call_proc
      store_unique_object obj
    end

    def construct_struct(str)
      modules = @modules; @modules = []
      symbols = []; values = []
      @call_proc = false
      sym = construct str
      store_unique_object sym
      @nested = true
      construct_integer(str).times do
        @call_proc = false
        symbols << construct(str)
        @call_proc = true if @proc
        values << construct(str)
      end
      obj = Struct.new(sym.to_s.sub(/\AStruct\:\:/, ''), *symbols).new
      (0...symbols.length).each do |i|
        obj[symbols[i]] = values[i]
      end
      set_instance_variables str, obj
      extend_object(modules, obj)
      @proc.call obj if @proc
      store_unique_object obj
    end

    def construct_symbol(str)
      obj = get_byte_sequence(str).to_sym
      @proc.call obj if @call_proc
      store_unique_object obj
    end

    def extend_object(modules, obj)
      modules.reverse_each do |sym|
        mod = Module.const_lookup sym
        obj.extend mod
      end

      obj
    end

    def find_link(obj)
      @links[obj.object_id]
    end

    def find_symlink(obj)
      @symlinks[obj.object_id]
    end

    def frexp(flt)
      ptr = MemoryPointer.new :int
      return Platform::Float.frexp flt, ptr
    ensure
      ptr.free if ptr
    end

    def get_byte_sequence(str)
      size = construct_integer(str)
      i = @consumed
      k = i + size
      @consumed += size
      str[i...k]
    end

    def get_module_names(obj)
      names = []
      sup = obj.metaclass.superclass

      while sup and [Module, IncludedModule].include? sup.class do
        names << sup.name
        sup = sup.superclass
      end

      names
    end

    def get_user_class
      cls = Module.const_lookup @user_class
      @user_class = nil
      cls
    end

    def ldexp(flt, exp)
      Platform::Float.ldexp flt, exp
    end

    def modf(flt)
      ptr = MemoryPointer.new :double

      flt = Platform::Float.modf flt, ptr
      num = ptr.read_float

      return flt, num
    ensure
      ptr.free if ptr
    end

    def prepare_ivar(sym)
      str = sym.to_s
      str.sub!(/\A\@+/, '')
      ('@' + str).to_sym
    end

    def serialize(obj)
      raise ArgumentError, "exceed depth limit" if @depth == 0

      # How much depth we have left.
      @depth -= 1;

      if link = find_link(obj)
        str = TYPE_LINK + serialize_integer(link)
      else
        add_object obj
        if obj.respond_to? :_dump then
          str = serialize_custom_object_ud obj
        elsif obj.respond_to? :marshal_dump then
          str = serialize_custom_object_mud obj
        else
          str = obj.to_marshal self
        end
      end

      @depth += 1

      return str
    end

    def serialize_custom_object_mud(obj)
      val = obj.marshal_dump
      out = TYPE_USRMARSHAL + serialize(obj.class.name.to_sym)
      out << val.to_marshal(self)
    end

    def serialize_custom_object_ud(obj)
      str = obj._dump @depth
      raise TypeError, "_dump() must return string" if str.class != String
      out = serialize_instance_variables_prefix(str)
      out << TYPE_USERDEF + serialize(obj.class.name.to_sym)
      out << serialize_integer(str.length) + str
      out << serialize_instance_variables_suffix(str)
    end

    def serialize_extended_object(obj)
      str = ''
      get_module_names(obj).each do |mod_name|
        str << TYPE_EXTENDED + serialize(mod_name.to_sym)
      end
      str
    end

    def serialize_float_thing(flt)
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

    def serialize_instance_variables_prefix(obj)
      if obj.instance_variables.length > 0
        TYPE_IVAR + ''
      else
      ''
      end
    end

    def serialize_instance_variables_suffix(obj, force = false)
      if force or obj.instance_variables.length > 0
        str = serialize_integer(obj.instance_variables.length)
        obj.instance_variables.each do |ivar|
          sym = ivar.to_sym
          val = obj.instance_variable_get(sym)
          str << serialize(sym)
          str << serialize(val)
        end
        str
      else
      ''
      end
    end

    def serialize_integer(n)
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

    def serialize_user_class(obj, cls)
      if obj.class != cls
        TYPE_UCLASS + serialize(obj.class.name.to_sym)
      else
      ''
      end
    end

    def set_instance_variables(str, obj)
      if @has_ivar
        @has_ivar = false
        @nested = true
        construct_integer(str).times do
          @call_proc = false
          sym = construct str
          @call_proc = true if @proc
          val = construct str
          obj.instance_variable_set(prepare_ivar(sym), val)
        end
      end
      obj
    end

    def store_unique_object(obj)
      if obj.kind_of? Symbol
        add_symlink obj
      else
        add_object obj
      end
      obj
    end

    def to_byte(n)
      [n].pack('C')
    end

  end

  def self.dump(obj, an_io=nil, limit=nil)
    ms = State.new

    if limit.nil?
      if an_io.kind_of? Fixnum
        limit = an_io
        an_io = nil
      else
        limit = -1
      end
    end

    ms.depth = Type.coerce_to limit, Fixnum, :to_int

    if an_io and !an_io.respond_to? :write
      raise TypeError, "output must respond to write"
    end

    str = VERSION_STRING + ms.serialize(obj)

    if an_io
      an_io.write(str)
      return an_io
    end

    return str
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

    ms.construct str
  end

end

