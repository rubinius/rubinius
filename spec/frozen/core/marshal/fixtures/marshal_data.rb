class UserDefined

  class Nested
    def ==(other)
      other.kind_of? self.class
    end
  end

  attr_reader :a, :b

  def initialize
    @a = 'stuff'
    @b = @a
  end

  def _dump(depth)
    Marshal.dump [@a, @b]
  end

  def self._load(data)
    a, b = Marshal.load data

    obj = allocate
    obj.instance_variable_set :@a, a
    obj.instance_variable_set :@b, b

    obj
  end

  def ==(other)
    self.class === other and
    @a == other.a and
    @b == other.b
  end

end

class UserDefinedWithIvar
  attr_reader :a, :b, :c

  def initialize
    @a = 'stuff'
    @a.instance_variable_set :@foo, :UserDefinedWithIvar
    @b = 'more'
    @c = @b
  end

  def _dump(depth)
    Marshal.dump [@a, @b, @c]
  end

  def self._load(data)
    a, b, c = Marshal.load data

    obj = allocate
    obj.instance_variable_set :@a, a
    obj.instance_variable_set :@b, b
    obj.instance_variable_set :@c, c

    obj
  end

  def ==(other)
    self.class === other and
    @a == other.a and
    @b == other.b and
    @c == other.c and
    @a.instance_variable_get(:@foo) == other.a.instance_variable_get(:@foo)
  end
end

class UserMarshal
  attr_reader :data

  def initialize
    @data = 'stuff'
  end
  def marshal_dump() @data end
  def marshal_load(data) @data = data end
  def ==(other) self.class === other and @data == other.data end
end

class UserMarshalWithIvar
  attr_reader :data

  def initialize
    @data = 'my data'
  end

  def marshal_dump
    [@data]
  end

  def marshal_load(o)
    @data = o.first
  end

  def ==(other)
    self.class === other and
    @data = other.data
  end
end

class UserArray < Array
end

class UserHash < Hash
end

class UserHashInitParams < Hash
  def initialize(a)
    @a = a
  end
end

class UserObject
end

class UserRegexp < Regexp
end

class UserString < String
end

module Meths
  def meths_method() end
end

module MethsMore
  def meths_more_method() end
end

Struct.new "Pyramid"
Struct.new "Useful", :a, :b

module MarshalSpec
  DATA = {
    "nil" => [nil, "\004\b0"],
    "1..2" => [(1..2),
               "\004\bo:\nRange\b:\nbegini\006:\texclF:\bendi\a",
               { :begin => 1, :end => 2, :exclude_end? => false }],
    "1...2" => [(1...2),
                "\004\bo:\nRange\b:\nbegini\006:\texclT:\bendi\a",
               { :begin => 1, :end => 2, :exclude_end? => true }],
    "'a'..'b'" => [('a'..'b'),
                   "\004\bo:\nRange\b:\nbegin\"\006a:\texclF:\bend\"\006b",
                   { :begin => 'a', :end => 'b', :exclude_end? => false }],
    "Struct" => [Struct::Useful.new(1, 2),
                 "\004\bS:\023Struct::Useful\a:\006ai\006:\006bi\a"],
    "Symbol" => [:symbol,
                 "\004\b:\vsymbol"],
    "true" => [true,
               "\004\bT"],
    "false" => [false,
                "\004\bF"],
    "String empty" => ['',
                       "\004\b\"\000"],
    "String small" => ['small',
                       "\004\b\"\012small"],
    "String big" => ['big' * 100,
                     "\004\b\"\002,\001#{'big' * 100}"],
    "String extended" => [''.extend(Meths), # TODO: check for module on load
                          "\004\be:\nMeths\"\000"],
    "String subclass" => [UserString.new,
                          "\004\bC:\017UserString\"\000"],
    "String subclass extended" => [UserString.new.extend(Meths),
                                   "\004\be:\nMethsC:\017UserString\"\000"],
    "Symbol small" => [:big,
                       "\004\b:\010big"],
    "Symbol big" => [('big' * 100).to_sym,
                               "\004\b:\002,\001#{'big' * 100}"],
    "Bignum -2**64" => [-2**64,
                        "\004\bl-\n\000\000\000\000\000\000\000\000\001\000"],
    "Bignum -2**63" => [-2**63,
                        "\004\bl-\t\000\000\000\000\000\000\000\200"],
    "Fixnum -2**24" => [-2**24,
                        "\004\bi\375\000\000\000"],
    "Fixnum -2**16" => [-2**16,
                        "\004\bi\376\000\000"],
    "Fixnum -2**8" => [-2**8,
                       "\004\bi\377\000"],
    "Fixnum -123" => [-123,
                      "\004\bi\200"],
    "Fixnum 0" => [0,
                   "\004\bi\000"],
    "Fixnum 5" => [5,
                   "\004\bi\n"],
    "Fixnum 2**8" => [2**8,
                      "\004\bi\002\000\001"],
    "Fixnum 2**16" => [2**16,
                       "\004\bi\003\000\000\001"],
    "Fixnum 2**24" => [2**24,
                       "\004\bi\004\000\000\000\001"],
    "Bignum 2**64" => [2**64,
                       "\004\bl+\n\000\000\000\000\000\000\000\000\001\000"],
    "Bignum 2**90" => [2**90,
                       "\004\bl+\v#{"\000" * 11}\004"],
    "Class String" => [String,
                       "\004\bc\vString"],
    "Module Marshal" => [Marshal,
                         "\004\bm\fMarshal"],
    "Module nested" => [UserDefined::Nested.new,
                        "\004\bo:\030UserDefined::Nested\000"],
    "_dump object" => [UserDefinedWithIvar.new,
                       "\004\bu:\030UserDefinedWithIvar5\004\b[\bI\"\nstuff\006:\t@foo:\030UserDefinedWithIvar\"\tmore@\a"],
    "_dump object extended" => [UserDefined.new.extend(Meths),
                                "\004\bu:\020UserDefined\022\004\b[\a\"\nstuff@\006"],
    "marshal_dump object" => [UserMarshalWithIvar.new,
                              "\004\bU:\030UserMarshalWithIvar[\006\"\fmy data"],
    "Regexp" => [/\A.\Z/,
                 "\004\b/\n\\A.\\Z\000"],
    "Regexp subclass /i" => [UserRegexp.new('', Regexp::IGNORECASE),
                             "\004\bC:\017UserRegexp/\000\001"],
    "Float 0.0" => [0.0,
                    "\004\bf\0060"],
    "Float -0.0" => [-0.0,
                     "\004\bf\a-0"],
    "Float Infinity" => [(1.0 / 0.0),
                         "\004\bf\binf"],
    "Float -Infinity" => [(-1.0 / 0.0),
                          "\004\bf\t-inf"],
    "Float 1.0" => [1.0,
                    "\004\bf\0061"],
    "Hash" => [Hash.new,
               "\004\b{\000"],
    "Hash subclass" => [UserHash.new,
                        "\004\bC:\rUserHash{\000"],
    "Array" => [Array.new,
                "\004\b[\000"],
    "Array subclass" => [UserArray.new,
                     "\004\bC:\016UserArray[\000"],
    "Struct" => [Struct::Pyramid.new,
                 "\004\bS:\024Struct::Pyramid\000"],
  }
end

