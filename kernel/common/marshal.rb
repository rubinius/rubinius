# -*- encoding: us-ascii -*-

class Object
  def __marshal__(ms, strip_ivars = false)
    out = ms.serialize_extended_object self
    out << "o"
    cls = Rubinius::Type.object_class self
    name = Rubinius::Type.module_inspect cls
    out << ms.serialize(name.to_sym)
    out << ms.serialize_instance_variables_suffix(self, true, strip_ivars)
  end
end

class Range
  def __marshal__(ms)
    super(ms, true)
  end
end

class NilClass
  def __marshal__(ms)
    Rubinius.binary_string("0")
  end
end

class TrueClass
  def __marshal__(ms)
    Rubinius.binary_string("T")
  end
end

class FalseClass
  def __marshal__(ms)
    Rubinius.binary_string("F")
  end
end

class Symbol
  def __marshal__(ms)
    if idx = ms.find_symlink(self)
      Rubinius.binary_string(";#{ms.serialize_integer(idx)}")
    else
      ms.add_symlink self
      ms.serialize_symbol(self)
    end
  end
end

class String
  def __marshal__(ms)
    out =  ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, String)
    out << ms.serialize_string(self)
    out << ms.serialize_instance_variables_suffix(self)
    out
  end
end

class Fixnum
  def __marshal__(ms)
    ms.serialize_integer(self, "i")
  end
end

class Bignum
  def __marshal__(ms)
    ms.serialize_bignum(self)
  end
end

class Regexp
  def __marshal__(ms)
    str = self.source
    out =  ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Regexp)
    out << "/"
    out << ms.serialize_integer(str.length) + str
    out << (options & 0x7).chr
    out << ms.serialize_instance_variables_suffix(self)

    out
  end
end

class Struct
  def __marshal__(ms)
    attr_syms = _attrs.map { |a| "@#{a}".to_sym }
    exclude = Rubinius.convert_to_names attr_syms

    out =  ms.serialize_instance_variables_prefix(self, exclude)
    out << ms.serialize_extended_object(self)

    out << "S"

    out << ms.serialize(self.class.name.to_sym)
    out << ms.serialize_integer(self.length)

    self.each_pair do |name, value|
      out << ms.serialize(name)
      out << ms.serialize(value)
    end

    out << ms.serialize_instance_variables_suffix(self, false, false, exclude)

    out
  end
end

class Array
  def __marshal__(ms)
    out =  ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Array)
    out << "["
    out << ms.serialize_integer(self.length)
    unless empty?
      each do |element|
        out << ms.serialize(element)
      end
    end
    out << ms.serialize_instance_variables_suffix(self)

    out
  end
end

class Hash
  def __marshal__(ms)
    raise TypeError, "can't dump hash with default proc" if default_proc

    exclude_syms = %w[
      @capacity @mask @max_entries @size @entries @default_proc @default
      @state @compare_by_identity @head @tail @table
    ].map { |a| a.to_sym }
    excluded_ivars = Rubinius.convert_to_names exclude_syms

    out =  ms.serialize_instance_variables_prefix(self, excluded_ivars)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Hash)
    out << (self.default ? "}" : "{")
    out << ms.serialize_integer(length)
    unless empty?
      each_pair do |key, val|
        out << ms.serialize(key)
        out << ms.serialize(val)
      end
    end
    out << (self.default ? ms.serialize(self.default) : '')
    out << ms.serialize_instance_variables_suffix(self, false, false,
                                                  excluded_ivars)

    out
  end
end

class Float
  def __marshal__(ms)
    str = if nan?
            "nan"
          elsif zero?
            (1.0 / self) < 0 ? '-0' : '0'
          elsif infinite?
            self < 0 ? "-inf" : "inf"
          else
            ("%.*g" % [17, self]) + ms.serialize_mantissa(self)
          end
    Rubinius.binary_string("f#{ms.serialize_integer(str.length)}#{str}")
  end
end

module Unmarshalable
  def __marshal__(ms)
    raise TypeError, "marshaling is undefined for class #{self.class}"
  end
end

class Method
  include Unmarshalable
end

class Proc
  include Unmarshalable
end

class IO
  include Unmarshalable
end

class MatchData
  include Unmarshalable
end

module Marshal

  MAJOR_VERSION = 4
  MINOR_VERSION = 8

  VERSION_STRING = "\x04\x08"

  # Here only for reference
  TYPE_NIL = ?0
  TYPE_TRUE = ?T
  TYPE_FALSE = ?F
  TYPE_FIXNUM = ?i

  TYPE_EXTENDED = ?e
  TYPE_UCLASS = ?C
  TYPE_OBJECT = ?o
  TYPE_DATA = ?d  # no specs
  TYPE_USERDEF = ?u
  TYPE_USRMARSHAL = ?U
  TYPE_FLOAT = ?f
  TYPE_BIGNUM = ?l
  TYPE_STRING = ?"
  TYPE_REGEXP = ?/
  TYPE_ARRAY = ?[
  TYPE_HASH = ?{
  TYPE_HASH_DEF = ?}
  TYPE_STRUCT = ?S
  TYPE_MODULE_OLD = ?M  # no specs
  TYPE_CLASS = ?c
  TYPE_MODULE = ?m

  TYPE_SYMBOL = ?:
  TYPE_SYMLINK = ?;

  TYPE_IVAR = ?I
  TYPE_LINK = ?@

  class State

    def initialize(stream, depth, proc)
      # shared
      @links = Rubinius::LookupTable.new
      @symlinks = Rubinius::LookupTable.new
      @symbols = []
      @objects = []

      # dumping
      @depth = depth

      # loading
      if stream
        @stream = stream
      else
        @stream = nil
      end

      if stream
        @consumed = 2
      else
        @consumed = 0
      end

      @modules = nil
      @has_ivar = []
      @proc = proc
      @call = true
    end

    def const_lookup(name, type = nil)
      mod = Object

      parts = String(name).split '::'
      parts.each { |part| mod = mod.const_get(part) }

      if type and not mod.instance_of? type
        raise ArgumentError, "#{name} does not refer to a #{type}"
      end

      mod
    end


    def add_object(obj)
      return if Rubinius::Type.object_kind_of? obj, ImmediateValue
      sz = @links.size
      @objects[sz] = obj
      @links[obj.__id__] = sz
    end

    def add_symlink(obj)
      sz = @symlinks.size
      @symbols[sz] = obj
      @symlinks[obj.__id__] = sz
    end

    def call(obj)
      @proc.call obj if @proc and @call
    end

    def construct(ivar_index = nil, call_proc = true)
      type = consume_byte()
      obj = case type
            when 48   # ?0
              nil
            when 84   # ?T
              true
            when 70   # ?F
              false
            when 99   # ?c
              construct_class
            when 109  # ?m
              construct_module
            when 77   # ?M
              construct_old_module
            when 105  # ?i
              construct_integer
            when 108  # ?l
              construct_bignum
            when 102  # ?f
              construct_float
            when 58   # ?:
              construct_symbol
            when 34   # ?"
              construct_string
            when 47   # ?/
              construct_regexp
            when 91   # ?[
              construct_array
            when 123  # ?{
              construct_hash
            when 125  # ?}
              construct_hash_def
            when 83   # ?S
              construct_struct
            when 111  # ?o
              construct_object
            when 117  # ?u
              construct_user_defined ivar_index
            when 85   # ?U
              construct_user_marshal
            when 100  # ?d
              construct_data
            when 64   # ?@
              num = construct_integer
              obj = @objects[num]

              raise ArgumentError, "dump format error (unlinked)" unless obj

              return obj
            when 59   # ?;
              num = construct_integer
              sym = @symbols[num]

              raise ArgumentError, "bad symbol" unless sym

              return sym
            when 101  # ?e
              @modules ||= []

              name = get_symbol
              @modules << const_lookup(name, Module)

              obj = construct nil, false

              extend_object obj

              obj
            when 67   # ?C
              name = get_symbol
              @user_class = name

              construct nil, false

            when 73   # ?I
              ivar_index = @has_ivar.length
              @has_ivar.push true

              obj = construct ivar_index, false

              set_instance_variables obj if @has_ivar.pop

              obj
            else
              raise ArgumentError, "load error, unknown type #{type}"
            end

      call obj if @proc and call_proc

      @stream.tainted? ? obj.taint : obj
    end

    def construct_class
      obj = const_lookup(get_byte_sequence.to_sym, Class)
      store_unique_object obj
      obj
    end

    def construct_module
      obj = const_lookup(get_byte_sequence.to_sym, Module)
      store_unique_object obj
      obj
    end

    def construct_old_module
      obj = const_lookup(get_byte_sequence.to_sym)
      store_unique_object obj
      obj
    end

    def construct_array
      obj = []
      store_unique_object obj

      if @user_class
        cls = get_user_class()
        if cls < Array
          Rubinius::Unsafe.set_class obj, cls
        else
          # This is what MRI does, it's weird.
          return cls.allocate
        end
      end

      construct_integer.times do |i|
        obj.__append__ construct
      end

      obj
    end

    def construct_bignum
      sign = consume_byte() == 45 ? -1 : 1  # ?-
      size = construct_integer * 2

      result = 0

      data = consume size
      (0...size).each do |exp|
        result += (data.getbyte(exp) * 2**(exp*8))
      end

      obj = result * sign

      store_unique_object obj
    end

    def construct_data
      name = get_symbol
      klass = const_lookup name, Class
      store_unique_object klass

      obj = klass.allocate

      # TODO ensure obj is a wrapped C pointer (T_DATA in MRI-land)

      store_unique_object obj

      unless obj.respond_to? :_load_data
        raise TypeError,
              "class #{name} needs to have instance method `_load_data'"
      end

      obj._load_data construct

      obj
    end

    def construct_float
      s = get_byte_sequence

      if s == "nan"
        obj = 0.0 / 0.0
      elsif s == "inf"
        obj = 1.0 / 0.0
      elsif s == "-inf"
        obj = 1.0 / -0.0
      else
        obj = s.to_f
      end

      store_unique_object obj

      obj
    end

    def construct_hash
      obj = @user_class ? get_user_class.allocate : {}
      store_unique_object obj

      construct_integer.times do
        key = construct
        val = construct

        # Use __store__ (an alias for []=) to get around subclass overrides
        obj.__store__ key, val
      end

      obj
    end

    def construct_hash_def
      obj = @user_class ? get_user_class.allocate : {}
      store_unique_object obj

      construct_integer.times do
        key = construct
        val = construct
        obj[key] = val
      end

      obj.default = construct

      obj
    end

    def construct_integer
      c = consume_byte()

      # The format appears to be a simple integer compression format
      #
      # The 0-123 cases are easy, and use one byte
      # We've read c as unsigned char in a way, but we need to honor
      # the sign bit. We do that by simply comparing with the +128 values
      return 0 if c == 0
      return c - 5 if 4 < c and c < 128

      # negative, but checked known it's instead in 2's compliment
      return c - 251 if 252 > c and c > 127

      # otherwise c (now in the 1 to 4 range) indicates how many
      # bytes to read to construct the value.
      #
      # Because we're operating on a small number of possible values,
      # it's cleaner to just unroll the calculate of each

      case c
      when 1
        consume_byte
      when 2
        consume_byte | (consume_byte << 8)
      when 3
        consume_byte | (consume_byte << 8) | (consume_byte << 16)
      when 4
        consume_byte | (consume_byte << 8) | (consume_byte << 16) |
                       (consume_byte << 24)

      when 255 # -1
        consume_byte - 256
      when 254 # -2
        (consume_byte | (consume_byte << 8)) - 65536
      when 253 # -3
        (consume_byte |
         (consume_byte << 8) |
         (consume_byte << 16)) - 16777216 # 2 ** 24
      when 252 # -4
        (consume_byte |
         (consume_byte << 8) |
         (consume_byte << 16) |
         (consume_byte << 24)) - 4294967296
      else
        raise "Invalid integer size: #{c}"
      end
    end

    def construct_object
      name = get_symbol
      klass = const_lookup name, Class
      obj = klass.allocate

      raise TypeError, 'dump format error' unless Object === obj

      store_unique_object obj
      set_instance_variables obj

      obj
    end

    def construct_regexp
      s = get_byte_sequence
      if @user_class
        obj = get_user_class.new s, consume_byte
      else
        obj = Regexp.new s, consume_byte
      end

      store_unique_object obj
    end

    def construct_string
      obj = get_byte_sequence
      obj = get_user_class.new obj if @user_class

      store_unique_object obj
    end

    def construct_struct
      symbols = []
      values = []

      name = get_symbol
      store_unique_object name

      klass = const_lookup name, Class
      members = klass.members

      obj = klass.allocate
      store_unique_object obj

      construct_integer.times do |i|
        slot = get_symbol
        unless members[i].intern == slot
          raise TypeError, "struct %s is not compatible (%p for %p)" %
            [klass, slot, members[i]]
        end

        obj.instance_variable_set "@#{slot}", construct
      end

      obj
    end

    def construct_symbol
      obj = get_byte_sequence.to_sym
      store_unique_object obj

      obj
    end

    def construct_user_defined(ivar_index)
      name = get_symbol
      klass = const_lookup name, Class

      data = get_byte_sequence

      if ivar_index and @has_ivar[ivar_index]
        set_instance_variables data
        @has_ivar[ivar_index] = false
      end

      obj = klass._load data

      store_unique_object obj

      obj
    end

    def construct_user_marshal
      name = get_symbol
      store_unique_object name

      klass = const_lookup name, Class
      obj = klass.allocate

      extend_object obj if @modules

      unless obj.respond_to? :marshal_load
        raise TypeError, "instance of #{klass} needs to have method `marshal_load'"
      end

      store_unique_object obj

      data = construct
      obj.marshal_load data

      obj
    end

    def extend_object(obj)
      obj.__extend__(@modules.pop) until @modules.empty?
    end

    def find_link(obj)
      @links[obj.__id__]
    end

    def find_symlink(obj)
      @symlinks[obj.__id__]
    end

    def get_byte_sequence
      size = construct_integer
      consume size
    end

    def get_user_class
      cls = const_lookup @user_class, Class
      @user_class = nil
      cls
    end

    def get_symbol
      type = consume_byte()

      case type
      when 58 # TYPE_SYMBOL
        @call = false
        obj = construct_symbol
        @call = true
        obj
      when 59 # TYPE_SYMLINK
        num = construct_integer
        @symbols[num]
      else
        raise ArgumentError, "expected TYPE_SYMBOL or TYPE_SYMLINK, got #{type.inspect}"
      end
    end

    def prepare_ivar(ivar)
      ivar.to_s =~ /\A@/ ? ivar : "@#{ivar}".to_sym
    end

    def serialize(obj)
      raise ArgumentError, "exceed depth limit" if @depth == 0

      # How much depth we have left.
      @depth -= 1;

      if link = find_link(obj)
        str = Rubinius.binary_string("@#{serialize_integer(link)}")
      else
        add_object obj

        # ORDER MATTERS.
        if obj.respond_to? :marshal_dump
          str = serialize_user_marshal obj
        elsif obj.respond_to? :_dump
          str = serialize_user_defined obj
        else
          str = obj.__marshal__ self
        end
      end

      @depth += 1

      Rubinius::Type.infect(str, obj)
    end

    def serialize_extended_object(obj)
      str = ''
      if mods = Rubinius.extended_modules(obj)
        mods.each do |mod|
          str << "e#{serialize(mod.name.to_sym)}"
        end
      end
      Rubinius.binary_string(str)
    end

    def serialize_mantissa(flt)
      str = ""
      flt = Math.modf(Math.ldexp(Math.frexp(flt.abs)[0], 37))[0]
      if flt > 0
        str = "\0" * 32
        i = 0
        while flt > 0
          flt, n = Math.modf(Math.ldexp(flt, 32))
          n = n.to_i
          str[i += 1] = (n >> 24) & 0xff
          str[i += 1] = (n >> 16) & 0xff
          str[i += 1] = (n >> 8) & 0xff
          str[i += 1] = (n & 0xff)
        end
        str.gsub!(/(\000)*\Z/, '')
      end
      Rubinius.binary_string(str)
    end

    def serializable_instance_variables(obj, exclude_ivars)
      ivars = obj.__instance_variables__
      ivars -= exclude_ivars if exclude_ivars
      ivars
    end

    def serialize_instance_variables_prefix(obj, exclude_ivars = false)
      ivars = serializable_instance_variables(obj, exclude_ivars)
      Rubinius.binary_string(!ivars.empty? || serialize_encoding?(obj) ? "I" : "")
    end

    def serialize_instance_variables_suffix(obj, force=false,
                                            strip_ivars=false,
                                            exclude_ivars=false)
      ivars = serializable_instance_variables(obj, exclude_ivars)

      unless force or !ivars.empty? or serialize_encoding?(obj)
        return Rubinius.binary_string("")
      end

      count = ivars.size

      if serialize_encoding?(obj)
        str = serialize_integer(count + 1)
        str << serialize_encoding(obj)
      else
        str = serialize_integer(count)
      end

      ivars.each do |ivar|
        sym = ivar.to_sym
        val = obj.__instance_variable_get__(sym)
        if strip_ivars
          str << serialize(ivar.to_s[1..-1].to_sym)
        else
          str << serialize(sym)
        end
        str << serialize(val)
      end

      Rubinius.binary_string(str)
    end

    def serialize_integer(n, prefix = nil)
      if (!Rubinius::L64 && n.is_a?(Fixnum)) || ((n >> 31) == 0 or (n >> 31) == -1)
        Rubinius.binary_string(prefix.to_s + serialize_fixnum(n))
      else
        serialize_bignum(n)
      end
    end

    def serialize_fixnum(n)
      if n == 0
        s = n.chr
      elsif n > 0 and n < 123
        s = (n + 5).chr
      elsif n < 0 and n > -124
        s = (256 + (n - 5)).chr
      else
        s = "\0"
        cnt = 0
        4.times do
          s << (n & 0xff).chr
          n >>= 8
          cnt += 1
          break if n == 0 or n == -1
        end
        s[0] = (n < 0 ? 256 - cnt : cnt).chr
      end
      Rubinius.binary_string(s)
    end

    def serialize_bignum(n)
      str = (n < 0 ? 'l-' : 'l+')
      cnt = 0
      num = n.abs

      while num != 0
        str << (num & 0xff).chr
        num >>= 8
        cnt += 1
      end

      if cnt % 2 == 1
        str << "\0"
        cnt += 1
      end

      Rubinius.binary_string(str[0..1] + serialize_fixnum(cnt / 2) + str[2..-1])
    end

    def serialize_symbol(obj)
      str = obj.to_s
      Rubinius.binary_string(":#{serialize_integer(str.length)}#{str}")
    end

    def serialize_string(str)
      Rubinius.binary_string("\"#{serialize_integer(str.length)}#{str}")
    end

    def serialize_user_class(obj, cls)
      if obj.class != cls
        Rubinius.binary_string("C#{serialize(obj.class.name.to_sym)}")
      else
        Rubinius.binary_string('')
      end
    end

    def serialize_user_defined(obj)
      str = obj._dump @depth

      unless str.kind_of? String
        raise TypeError, "_dump() must return string"
      end

      out = serialize_instance_variables_prefix(str)
      out << Rubinius.binary_string("u#{serialize(obj.class.name.to_sym)}")
      out << serialize_integer(str.length) + str
      out << serialize_instance_variables_suffix(str)

      out
    end

    def serialize_user_marshal(obj)
      val = obj.marshal_dump

      add_object val

      cls = Rubinius::Type.object_class obj
      name = Rubinius::Type.module_inspect cls
      Rubinius.binary_string("U#{serialize(name.to_sym)}#{val.__marshal__(self)}")
    end

    def set_instance_variables(obj)
      construct_integer.times do
        ivar = get_symbol
        value = construct
        obj.__instance_variable_set__ prepare_ivar(ivar), value
      end
    end

    def store_unique_object(obj)
      if Symbol === obj
        add_symlink obj
      else
        add_object obj
      end
      obj
    end

  end

  class IOState < State
    def consume(bytes)
      @stream.read(bytes)
    end

    def consume_byte
      b = @stream.getbyte
      raise EOFError unless b
      b
    end
  end

  class StringState < State
    def initialize(stream, depth, prc)
      super stream, depth, prc

      if @stream
        @byte_array = stream.data
      end

    end

    def consume(bytes)
      raise ArgumentError, "marshal data too short" if @consumed > @stream.size
      data = @stream[@consumed, bytes]
      @consumed += bytes
      data
    end

    def consume_byte
      raise ArgumentError, "marshal data too short" if @consumed >= @stream.size
      data = @byte_array[@consumed]
      @consumed += 1
      return data
    end
  end

  def self.dump(obj, an_io=nil, limit=nil)
    unless limit
      if an_io.kind_of? Fixnum
        limit = an_io
        an_io = nil
      else
        limit = -1
      end
    end

    depth = Rubinius::Type.coerce_to limit, Fixnum, :to_int
    ms = State.new nil, depth, nil

    if an_io and !an_io.respond_to? :write
      raise TypeError, "output must respond to write"
    end

    str = Rubinius.binary_string(VERSION_STRING) + ms.serialize(obj)

    if an_io
      an_io.write(str)
      return an_io
    end

    return str
  end

  def self.load(obj, prc = nil)
    if obj.respond_to? :to_str
      data = obj.to_s

      major = data.getbyte 0
      minor = data.getbyte 1

      ms = StringState.new data, nil, prc

    elsif obj.respond_to?(:read) and obj.respond_to?(:getc)
      ms = IOState.new obj, nil, prc

      major = ms.consume_byte
      minor = ms.consume_byte
    else
      raise TypeError, "instance of IO needed"
    end

    if major != MAJOR_VERSION or minor > MINOR_VERSION
      raise TypeError, "incompatible marshal file format (can't be read)\n\tformat version #{MAJOR_VERSION}.#{MINOR_VERSION} required; #{major.inspect}.#{minor.inspect} given"
    end

    ms.construct
  rescue NameError => e
    raise ArgumentError, e.message, e
  end

  class << self
    alias_method :restore, :load
  end

end
