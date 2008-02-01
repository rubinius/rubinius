require File.dirname(__FILE__) + '/../../spec_helper'

mv = "\x04"  # Marshal::MAJOR_VERSION
nv = "\x08"  # Marshal::MINOR_VERSION

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

describe "Marshal.load" do

  it "loads a nil" do
    Marshal.load("\004\b0").should ==
      nil
  end

  it "loads a .. Numeric Range" do
    Marshal.load("\004\bo:\nRange\b:\nbegini\006:\texclF:\bendi\a").should ==
      (1..2)
  end

  it "loads a ... Numeric Range" do
    Marshal.load("\004\bo:\nRange\b:\nbegini\006:\texclT:\bendi\a").should ==
      (1...2)
  end

  it "loads a .. String Range" do
    Marshal.load("\004\bo:\nRange\b:\nbegin\"\006a:\texclF:\bend\"\006b").should ==
      ('a'..'b')
  end

  it "loads a Struct" do
    Marshal.load("\004\bS:\023Struct::Useful\a:\006ai\006:\006bi\a").should ==
      Struct::Useful.new(1, 2)
  end

  it "loads a Symbol" do
    Marshal.load("\004\b:\vsymbol").should ==
      :symbol
  end

  it "loads a true" do
    Marshal.load("\004\bT").should ==
      true
  end

  it "loads a false" do
    Marshal.load("\004\bF").should ==
      false
  end

  it "loads a \"\"" do
    Marshal.load("\004\b\"\000").should ==
      ''
  end

  it "loads a string of length 300" do
    Marshal.load("\004\b\"\002,\001aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").should ==
      'a' * 300
  end

  it "loads an extended String" do
    obj = ''.extend(Meths)
    Marshal.load("\004\be:\nMeths\"\000").should ==
      obj
  end

  it "loads a user_string" do
    Marshal.load("\004\bC:\017UserString\"\000").should ==
      UserString.new
  end

  it "loads an extended user string" do
    obj = UserString.new.extend(Meths)
    Marshal.load("\004\be:\nMethsC:\017UserString\"\000").should ==
      obj
  end

  it "loads a string having ivar with ref to self" do
    obj = 'hi'
    obj.instance_variable_set(:@self, obj)
    Marshal.load("\004\bI\"\ahi\006:\n@self@\000").should ==
      obj
  end

  it "loads a symbol of length 1" do
    Marshal.load("\004\b:\006a").should ==
      :a
  end

  it "loads a symbol of length 300" do
    obj = ('a' * 300).to_sym
    Marshal.load("\004\b:\002,\001aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").should ==
      obj
  end

  it "loads a fixnum 0" do
    Marshal.load("\004\bi\000").should ==
      0
  end

  it "loads a fixnum 5" do
    Marshal.load("\004\bi\n").should ==
      5
  end

  it "loads a fixnum -123" do
    Marshal.load("\004\bi\200").should ==
      -123
  end

  it "loads a fixnum 2**8" do
    Marshal.load("\004\bi\002\000\001").should ==
      2**8
  end

  it "loads a fixnum -2**8" do
    Marshal.load("\004\bi\377\000").should ==
      -2**8
  end

  it "loads a fixnum 2**24" do
    Marshal.load("\004\bi\004\000\000\000\001").should ==
      2**24
  end

  it "loads a fixnum (2**30 - 1)" do
    Marshal.load("\004\bi\004\377\377\377?").should ==
      2**30 - 1
  end

  it "loads a fixnum 2**16" do
    Marshal.load("\004\bi\003\000\000\001").should ==
      2**16
  end

  it "loads a fixnum -2**16" do
    Marshal.load("\004\bi\376\000\000").should ==
      -2**16
  end

  it "loads a fixnum -(2**16 - 1)" do
    obj = -(2**16 - 1)
    Marshal.load("\004\bi\376\001\000").should ==
      obj
  end

  it "loads a fixnum -2**30" do
    Marshal.load("\004\bi\374\000\000\000\300").should ==
      -2**30
  end

  it "loads a bignum 2**30" do
    Marshal.load("\004\bl+\a\000\000\000@").should ==
      2**30
  end

  it "loads a bignum -(2**31 - 1)" do
    obj = -(2**31 - 1)
    Marshal.load("\004\bl-\a\377\377\377\177").should ==
      obj
  end

  it "loads a bignum 2**40" do
    Marshal.load("\004\bl+\b\000\000\000\000\000\001").should ==
      2**40
  end

  it "loads a bignum -2**63" do
    Marshal.load("\004\bl-\t\000\000\000\000\000\000\000\200").should ==
      -2**63
  end

  it "loads a bignum 2**64" do
    Marshal.load("\004\bl+\n\000\000\000\000\000\000\000\000\001\000").should ==
      2**64
  end

  it "loads a bignum -2**64" do
    Marshal.load("\004\bl-\n\000\000\000\000\000\000\000\000\001\000").should ==
      -2**64
  end

  it "loads a bignum 2**90" do
    Marshal.load("\004\bl+\v\000\000\000\000\000\000\000\000\000\000\000\004").should ==
      2**90
  end

  it "loads a class" do
    Marshal.load("\004\bc\vString").should ==
      String
  end

  it "loads a module" do
    Marshal.load("\004\bm\fMarshal").should ==
      Marshal
  end

  it "loads a nested module" do
    obj = UserDefined::Nested.new
    Marshal.load("\004\bo:\030UserDefined::Nested\000").should ==
      obj
  end

  it "loads a user-defined marshaled object" do
    obj = UserDefinedWithIvar.new

    Marshal.load("\004\bu:\030UserDefinedWithIvar5\004\b[\bI\"\nstuff\006:\t@foo:\030UserDefinedWithIvar\"\tmore@\a").should ==
      obj
  end

  it "loads a user-defined marshaled and extended object" do
    obj = UserDefined.new.extend(Meths)
    Marshal.load("\004\bu:\020UserDefined\022\004\b[\a\"\nstuff@\006").should ==
      obj
  end

  it "loads a user-marshaled object" do
    obj = UserMarshalWithIvar.new
    Marshal.load("\004\bU:\030UserMarshalWithIvar[\006\"\fmy data").should ==
      obj
  end

  it "loads a user-marshaled extended object" do
    obj = UserMarshal.new.extend(Meths)

    new_obj = Marshal.load "\004\bU:\020UserMarshal\"\nstuff" 

    new_obj.should == obj

    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first.should == UserMarshal
  end

  it "loads a object" do
    Marshal.load("\004\bo:\vObject\000").class.should ==
      Object
  end

  it "loads a user_object" do
    obj = UserObject.new
    Marshal.load("\004\bo:\017UserObject\000").class.should ==
      UserObject
  end

  it "loads an extended Object" do
    obj = Object.new.extend(Meths)

    new_obj = Marshal.load "\004\be:\nMethso:\vObject\000"

    new_obj.class.should == obj.class
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first(2).should == [Meths, Object]
  end

  it "loads a object having ivar" do
    s = 'hi'
    arr = [:so, :so, s, s]
    obj = Object.new
    obj.instance_variable_set :@str, arr

    new_obj = Marshal.load "\004\bo:\vObject\006:\t@str[\t:\aso;\a\"\ahi@\a"
    new_str = new_obj.instance_variable_get :@str

    new_str.should == arr
  end

  it "loads a regexp" do
    obj = /\A.\Z/
    Marshal.load("\004\b/\n\\A.\\Z\000").should ==
      obj
  end

  it "loads an extended Regexp" do
    obj = /[a-z]/.extend(Meths, MethsMore)
    new_obj = Marshal.load "\004\be:\nMethse:\016MethsMore/\n[a-z]\000"

    new_obj.should == obj
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first(3).should ==
      [Meths, MethsMore, Regexp]
  end

  it "loads a user_regexp having option Regexp::IGNORECASE" do
    obj = UserRegexp.new('', Regexp::IGNORECASE)
    Marshal.load("\004\bC:\017UserRegexp/\000\001").should ==
      obj
  end

  it "loads a extended_user_regexp having ivar" do
    obj = UserRegexp.new('').extend(Meths)
    obj.instance_variable_set(:@noise, 'much')

    new_obj = Marshal.load "\004\bIe:\nMethsC:\017UserRegexp/\000\000\006:\v@noise\"\tmuch"

    new_obj.should == obj
    new_obj.instance_variable_get(:@noise).should == 'much'
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first(3).should ==
      [Meths, UserRegexp, Regexp]
  end

  it "loads a Float 0.0" do
    Marshal.load("\004\bf\0060").should ==
      0.0
  end

  it "loads a Float -0.0" do
    Marshal.load("\004\bf\a-0").should ==
      -0.0
  end

  it "loads a Float NaN" do
    obj = 0.0 / 0.0
    Marshal.load("\004\bf\bnan").to_s.should == 'NaN'
  end

  it "loads a Float Infinity" do
    obj = 1.0 / 0.0
    Marshal.load("\004\bf\binf").should ==
      obj
  end

  it "loads a Float -Infinity" do
    obj = -1.0 / 0.0
    Marshal.load("\004\bf\t-inf").should ==
      obj
  end

  it "loads a Float 1.0" do
    Marshal.load("\004\bf\0061").should ==
      1.0
  end

  it "loads a Float 1.3" do
    Marshal.load("\004\bf\v1.3\000\314\315").should ==
      1.3
  end

  it "loads a Float -5.1867345e-22" do
    obj = -5.1867345e-22
    Marshal.load("\004\bf\037-5.1867345000000008e-22\000\203_").should \
      be_close(obj, 1e-30)
  end

  it "loads a Float 1.1867345e+22" do
    obj = 1.1867345e+22
    Marshal.load("\004\bf\0361.1867344999999999e+22\000\344@").should ==
      obj
  end

  it "loads a hash" do
    Marshal.load("\004\b{\000").should ==
      Hash.new
  end

  it "loads a user_hash" do
    obj = UserHash.new
    Marshal.load("\004\bC:\rUserHash{\000").should ==
      obj
  end

  it "loads a array" do
    Marshal.load("\004\b[\000").should ==
      Array.new
  end

  it "loads a array containing objects having _dump method, and with proc" do
    arr = []
    proc = Proc.new { |o| arr << o }
    o1 = UserDefined.new; o2 = UserDefinedWithIvar.new
    obj = [o1, o2, o1, o2]

    Marshal.load "\004\b[\tu:\020UserDefined\022\004\b[\a\"\nstuff@\006u:\030UserDefinedWithIvar5\004\b[\bI\"\nstuff\006:\t@foo:\030UserDefinedWithIvar\"\tmore@\a@\006@\a", proc

    arr.should == [o1, o2, obj]
  end

  it "loads an array containing objects having marshal_dump method, and with proc" do
    arr = []
    proc = Proc.new { |o| arr << o }
    o1 = UserMarshal.new
    o2 = UserMarshalWithIvar.new
    obj = [o1, o2, o1, o2]

    Marshal.load "\004\b[\tU:\020UserMarshal\"\nstuffU:\030UserMarshalWithIvar[\006\"\fmy data@\006@\b", proc

    arr.should == ['stuff', o1, 'my data', ['my data'], o2, obj]
  end

  it "loads an Array with proc" do
    arr = []
    s = 'hi'
    s.instance_variable_set(:@foo, 5)
    st = Struct.new("Brittle", :a).new
    st.instance_variable_set(:@clue, 'none')
    st.a = 0.0
    h = Hash.new('def')
    h['nine'] = 9
    a = [:a, :b, :c]
    a.instance_variable_set(:@two, 2)
    obj = [s, 10, s, s, st, h, a]
    obj.instance_variable_set(:@zoo, 'ant')

    proc = Proc.new { |o| arr << o }
    new_obj = Marshal.load "\004\bI[\fI\"\ahi\006:\t@fooi\ni\017@\006@\006IS:\024Struct::Brittle\006:\006af\0060\006:\n@clue\"\tnone}\006\"\tninei\016\"\bdefI[\b;\a:\006b:\006c\006:\t@twoi\a\006:\t@zoo\"\bant", proc

    new_obj.should == obj
    new_obj.instance_variable_get(:@zoo).should == 'ant'

    arr.should ==
      [5, s, 10, 0.0, 'none', st, 'nine', 9, 'def', h, :b, :c, 2, a, 'ant', obj]
  end

  it "loads a array containing the same objects" do
    s = 'oh'; b = 'hi'; r = //; d = [b, :no, s, :go]; c = String; f = 1.0
    o1 = UserMarshalWithIvar.new; o2 = UserMarshal.new
    obj = [:so, 'hello', 100, :so, :so, d, :so, o2, :so, :no, o2,
           :go, c, nil, Struct::Pyramid.new, f, :go, :no, s, b, r,
           :so, 'huh', o1, true, b, b, 99, r, b, s, :so, f, c, :no, o1, d]

    Marshal.load("\004\b[*:\aso\"\nhelloii;\000;\000[\t\"\ahi:\ano\"\aoh:\ago;\000U:\020UserMarshal\"\nstuff;\000;\006@\n;\ac\vString0S:\024Struct::Pyramid\000f\0061;\a;\006@\t@\b/\000\000;\000\"\bhuhU:\030UserMarshalWithIvar[\006\"\fmy dataT@\b@\bih@\017@\b@\t;\000@\016@\f;\006@\021@\a").should ==
      obj
  end

  it "loads an array having ivar" do
    s = 'well'
    s.instance_variable_set(:@foo, 10)
    obj = ['5', s, 'hi'].extend(Meths, MethsMore)
    obj.instance_variable_set(:@mix, s)
    Marshal.load("\004\bI[\b\"\0065I\"\twell\006:\t@fooi\017\"\ahi\006:\t@mix@\a").should ==
      obj
  end

  it "loads a user_array" do
    obj = UserArray.new
    Marshal.load("\004\bC:\016UserArray[\000").should ==
      obj
  end

  it "loads a struct" do
    obj = Struct.new("Ure0").new
    Marshal.load("\004\bS:\021Struct::Ure0\000").should ==
      obj
  end

  it "loads a struct having ivar" do
    obj = Struct.new("Thick").new
    obj.instance_variable_set(:@foo, 5)
    Marshal.load("\004\bIS:\022Struct::Thick\000\006:\t@fooi\n").should ==
      obj
  end

  it "loads a struct having fields" do
    obj = Struct.new("Ure1", :a, :b).new
    Marshal.load("\004\bS:\021Struct::Ure1\a:\006a0:\006b0").should ==
      obj
  end

  it "loads a extended_struct having fields with same objects" do
    s = 'hi'
    obj = Struct.new("Ure2", :a, :b).new.extend(Meths)
    obj.a = [:a, s]; obj.b = [:Meths, s]

    Marshal.load("\004\be:\nMethsS:\021Struct::Ure2\a:\006a[\a;\a\"\ahi:\006b[\a;\000@\a").should ==
      obj
  end

end

