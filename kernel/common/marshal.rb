class Object
  def __marshal__(ms, strip_ivars = false)
    out = ms.serialize_extended_object self
    out << "o"
    name = self.__class__.__name__.to_sym
    out << ms.serialize(name)
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
    "0"
  end
end

class TrueClass
  def __marshal__(ms)
    "T"
  end
end

class FalseClass
  def __marshal__(ms)
    "F"
  end
end

class Class
  def __marshal__(ms)
    if __metaclass_object__
      raise TypeError, "singleton class can't be dumped"
    elsif name.empty?
      raise TypeError, "can't dump anonymous module #{self}"
    end

    "c#{ms.serialize_integer(name.length)}#{name}"
  end
end

class Module
  def __marshal__(ms)
    raise TypeError, "can't dump anonymous module #{self}" if name.empty?
    "m#{ms.serialize_integer(name.length)}#{name}"
  end
end

class Symbol
  def __marshal__(ms)
    if idx = ms.find_symlink(self) then
      ";#{ms.serialize_integer(idx)}"
    else
      ms.add_symlink self

      str = to_s
      ":#{ms.serialize_integer(str.length)}#{str}"
    end
  end
end

class String
  def __marshal__(ms)
    out =  ms.serialize_instance_variables_prefix(self)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, String)
    out << '"'
    out << ms.serialize_integer(self.length) << self
    out << ms.serialize_instance_variables_suffix(self)

    out
  end
end

class Fixnum
  def __marshal__(ms)
    "i#{ms.serialize_integer(self)}"
  end
end

class Bignum
  def __marshal__(ms)
    str = (self < 0 ? 'l-' : 'l+')
    cnt = 0
    num = self.abs

    while num != 0
      str << (num & 0xff).chr
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
    exclude = _attrs.map { |a| "@#{a}" }

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
    unless empty? then
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

    #excluded_ivars = %w[@bins @count @records]
    excluded_ivars = %w[@capacity @mask @max_entries @size @entries]

    out =  ms.serialize_instance_variables_prefix(self, excluded_ivars)
    out << ms.serialize_extended_object(self)
    out << ms.serialize_user_class(self, Hash)
    out << (self.default ? "}" : "{")
    out << ms.serialize_integer(length)
    unless empty? then
      each_pair do |(key, val)|
        out << ms.serialize(key)
        out << ms.serialize(val)
      end
    end
    out << (default ? ms.serialize(default) : '')
    out << ms.serialize_instance_variables_suffix(self, false, false,
                                                  excluded_ivars)

    out
  end
end

class Float
  def __marshal__(ms)
    str = if nan? then
            "nan"
          elsif zero? then
            (1.0 / self) < 0 ? '-0' : '0'
          elsif infinite? then
            self < 0 ? "-inf" : "inf"
          else
            # HACK shouldn't ignore the mantissa.
            ("%.*g" % [17, self]) #  + ms.serialize_mantissa(self)
          end
    "f#{ms.serialize_integer(str.length)}#{str}"
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
        @byte_array = stream.data
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

    def add_object(obj)
      return if obj.__kind_of__(ImmediateValue)
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
            when ?0
              nil
            when ?T
              true
            when ?F
              false
            when ?c, ?m
              # Don't use construct_symbol, because we must not
              # memoize this symbol.
              name = get_byte_sequence.to_sym
              obj = Object.const_lookup name

              store_unique_object obj

              obj
            when ?i
              construct_integer
            when ?l
              construct_bignum
            when ?f
              construct_float
            when ?:
              construct_symbol
            when ?"
              construct_string
            when ?/
              construct_regexp
            when ?[
              construct_array
            when ?{
              construct_hash
            when ?}
              construct_hash_def
            when ?S
              construct_struct
            when ?o
              construct_object
            when ?u
              construct_user_defined ivar_index
            when ?U
              construct_user_marshal
            when ?@
              num = construct_integer
              obj = @objects[num]

              raise ArgumentError, "dump format error (unlinked)" unless obj

              return obj
            when ?;
              num = construct_integer
              sym = @symbols[num]

              raise ArgumentError, "bad symbol" unless sym

              return sym
            when ?e
              @modules ||= []

              name = get_symbol
              @modules << Object.const_lookup(name)

              obj = construct nil, false

              extend_object obj

              obj
            when ?C
              name = get_symbol
              @user_class = name

              construct nil, false

            when ?I
              ivar_index = @has_ivar.length
              @has_ivar.push true

              obj = construct ivar_index, false

              set_instance_variables obj if @has_ivar.pop

              obj
            else
              raise ArgumentError, "load error, unknown type #{type}"
            end

      call obj if @proc and call_proc

      obj
    end

    def construct_array
      obj = @user_class ? get_user_class.new : []
      store_unique_object obj

      construct_integer.times do
        obj << construct
      end

      obj
    end

    def construct_bignum
      sign = consume_byte() == ?- ? -1 : 1
      size = construct_integer * 2

      result = 0

      data = consume size
      (0...size).each do |exp|
        result += (data[exp] * 2**(exp*8))
      end

      obj = result * sign

      store_unique_object obj
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
      klass = Object.const_lookup name
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

      klass = Object.const_lookup name
      members = klass.members

      obj = klass.allocate
      store_unique_object obj

      construct_integer.times do |i|
        slot = get_symbol
        unless members[i].intern == slot then
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
      klass = Module.const_lookup name

      data = get_byte_sequence

      if ivar_index and @has_ivar[ivar_index] then
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

      klass = Module.const_lookup name
      obj = klass.allocate

      extend_object obj if @modules

      unless obj.respond_to? :marshal_load then
        raise TypeError, "instance of #{klass} needs to have method `marshal_load'"
      end

      store_unique_object obj

      data = construct
      obj.marshal_load data

      obj
    end

    def consume(bytes)
      data = @stream[@consumed, bytes]
      @consumed += bytes
      data
    end

    def consume_byte
      data = @byte_array[@consumed]
      @consumed += 1
      return data
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
      cls = Module.const_lookup @user_class
      @user_class = nil
      cls
    end

    def get_symbol
      type = consume_byte()

      case type
      when TYPE_SYMBOL then
        @call = false
        obj = construct_symbol
        @call = true
        obj
      when TYPE_SYMLINK then
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
        str = "@#{serialize_integer(link)}"
      else
        add_object obj

        # ORDER MATTERS.
        if obj.respond_to? :marshal_dump then
          str = serialize_user_marshal obj
        elsif obj.respond_to? :_dump then
          str = serialize_user_defined obj
        else
          str = obj.__marshal__ self
        end
      end

      @depth += 1

      return str
    end

    def serialize_extended_object(obj)
      str = ''
      if mods = Rubinius.extended_modules(obj)
        mods.each do |mod|
          str << "e#{serialize(mod.name.to_sym)}"
        end
      end
      str
    end

    def serialize_mantissa(flt)
      str = "\0" * 32

      flt = Math.modf(Math.ldexp(Math.frexp(flt.abs)[0], 37))[0]
      p :inner => flt
      if flt > 0
        i = 1
        while flt > 0
          flt, n = Math.modf(Math.ldexp(flt, 32))
          n = n.to_i
          p :mant => n
          str[i += 1] = (n >> 24) & 0xff
          str[i += 1] = (n >> 16) & 0xff
          str[i += 1] = (n >> 8) & 0xff
          str[i += 1] = (n & 0xff)
        end

        str.chomp!("\0")
      end
      str
    end

    def serialize_instance_variables_prefix(obj, exclude_ivars = false)
      ivars = obj.__instance_variables__

      ivars -= exclude_ivars if exclude_ivars

      if ivars.length > 0 then
        "I"
      else
        ''
      end
    end

    def serialize_instance_variables_suffix(obj, force=false,
                                            strip_ivars=false,
                                            exclude_ivars=false)
      ivars = obj.__instance_variables__

      ivars -= exclude_ivars if exclude_ivars

      if force or !ivars.empty?
        str = serialize_integer(ivars.size)
        ivars.each do |ivar|
          sym = ivar.to_sym
          val = obj.__instance_variable_get__(sym)
          if strip_ivars
            str << serialize(ivar[1..-1].to_sym)
          else
            str << serialize(sym)
          end
          str << serialize(val)
        end
        str
      else
      ''
      end
    end

    def serialize_integer(n)
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
      s
    end

    def serialize_user_class(obj, cls)
      if obj.class != cls
        "C#{serialize(obj.class.name.to_sym)}"
      else
        ''
      end
    end

    def serialize_user_defined(obj)
      str = obj._dump @depth

      unless str.kind_of? String
        raise TypeError, "_dump() must return string"
      end

      out = serialize_instance_variables_prefix(str)
      out << "u#{serialize(obj.class.name.to_sym)}"
      out << serialize_integer(str.length) + str
      out << serialize_instance_variables_suffix(str)

      out
    end

    def serialize_user_marshal(obj)
      val = obj.marshal_dump

      add_object val

      "U#{serialize(obj.class.__name__.to_sym)}#{val.__marshal__(self)}"
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

  def self.dump(obj, an_io=nil, limit=nil)
    unless limit
      if an_io.kind_of? Fixnum
        limit = an_io
        an_io = nil
      else
        limit = -1
      end
    end

    depth = Type.coerce_to limit, Fixnum, :to_int
    ms = State.new nil, depth, nil

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
    if obj.respond_to? :to_str
      data = obj.to_s
    elsif obj.respond_to? :read
      data = obj.read
      if data.empty?
        raise EOFError, "end of file reached"
      end
    elsif obj.respond_to? :getc  # FIXME - don't read all of it upfront
      data = ''
      data << c while (c = obj.getc.chr)
    else
      raise TypeError, "instance of IO needed"
    end

    major = data[0]
    minor = data[1]

    if major != MAJOR_VERSION or minor > MINOR_VERSION then
      raise TypeError, "incompatible marshal file format (can't be read)\n\tformat version #{MAJOR_VERSION}.#{MINOR_VERSION} required; #{major}.#{minor} given"
    end

    ms = State.new data, nil, proc
    ms.construct
  end

end

