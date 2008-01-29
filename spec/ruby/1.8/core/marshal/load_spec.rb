require File.dirname(__FILE__) + '/../../spec_helper'

mv = "\x04"  # Marshal::MAJOR_VERSION
nv = "\x08"  # Marshal::MINOR_VERSION

describe "Marshal.load" do
  it "returns a Ruby Object from conversion of the given serialized data" do
    Marshal.load(Marshal.dump("abc")).should == "abc"
    Marshal.load(Marshal.dump(0..100)).should == (0..100)
    Marshal.load(Marshal.dump(0...100)).should == (0...100)
    Marshal.load(Marshal.dump(:symbol)).should == :symbol
    Marshal.load(Marshal.dump(Object)).should == Object
    Marshal.load(Marshal.dump([[], []])).should == [[], []]
  end
end

describe "Marshal.load with serialized nil" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(nil))).should == Marshal.dump(nil)
  end
end

describe "Marshal.load with serialized true" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(true))).should == Marshal.dump(true)
  end
end

describe "Marshal.load with serialized false" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(false))).should == Marshal.dump(false)
  end
end

describe "Marshal.load with serialized empty string" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(''))).should == Marshal.dump('')
  end
end

describe "Marshal.load with serialized string of length 300" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump('a' * 300))).should == Marshal.dump('a' * 300)
  end
end

module Meths
end
module MethsMore
end

describe "Marshal.load with serialized extended_string" do
  it "returns a construction of the argument" do
    obj = ''.extend(Meths)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

class UserString < String
end

describe "Marshal.load with serialized user_string" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(UserString.new))).should == Marshal.dump(UserString.new)
  end
end

describe "Marshal.load with serialized extended_user_string" do
  it "returns a construction of the argument" do
    obj = UserString.new.extend(Meths)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized string having ivar with ref to self" do
  it "returns a construction of the argument" do
    obj = 'hi'
    obj.instance_variable_set(:@self, obj)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized symbol of length 1" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(:a))).should == Marshal.dump(:a)
  end
end

describe "Marshal.load with serialized symbol of length 300" do
  it "returns a construction of the argument" do
    obj = ('a' * 300).to_sym
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized fixnum 0" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(0))).should == Marshal.dump(0)
  end
end

describe "Marshal.load with serialized fixnum 5" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(5))).should == Marshal.dump(5)
  end
end

describe "Marshal.load with serialized fixnum -123" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-123))).should == Marshal.dump(-123)
  end
end

describe "Marshal.load with serialized fixnum 2**8" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**8))).should == Marshal.dump(2**8)
  end
end

describe "Marshal.load with serialized fixnum -2**8" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-2**8))).should == Marshal.dump(-2**8)
  end
end

describe "Marshal.load with serialized fixnum 2**24" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**24))).should == Marshal.dump(2**24)
  end
end

describe "Marshal.load with serialized fixnum (2**30 - 1)" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**30 - 1))).should == Marshal.dump(2**30 - 1)
  end
end

describe "Marshal.load with serialized fixnum 2**16" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**16))).should == Marshal.dump(2**16)
  end
end

describe "Marshal.load with serialized fixnum -2**16" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-2**16))).should == Marshal.dump(-2**16)
  end
end

describe "Marshal.load with serialized fixnum -(2**16 - 1)" do
  it "returns a construction of the argument" do
    obj = -(2**16 - 1)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized fixnum -2**30" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-2**30))).should == Marshal.dump(-2**30)
  end
end

describe "Marshal.load with serialized bignum 2**30" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**30))).should == Marshal.dump(2**30)
  end
end

describe "Marshal.load with serialized bignum -(2**31 - 1)" do
  it "returns a construction of the argument" do
    obj = -(2**31 - 1)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized bignum 2**40" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**40))).should == Marshal.dump(2**40)
  end
end

describe "Marshal.load with serialized bignum -2**63" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-2**63))).should == Marshal.dump(-2**63)
  end
end

describe "Marshal.load with serialized bignum 2**64" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**64))).should == Marshal.dump(2**64)
  end
end

describe "Marshal.load with serialized bignum -2**64" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-2**64))).should == Marshal.dump(-2**64)
  end
end

describe "Marshal.load with serialized bignum 2**90" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(2**90))).should == Marshal.dump(2**90)
  end
end

describe "Marshal.load with serialized class" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(String))).should == Marshal.dump(String)
  end
end

describe "Marshal.load with serialized module" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(Marshal))).should == Marshal.dump(Marshal)
  end
end

class Custom
  class Nested
    def ==(other)
      other.kind_of? self.class
    end
  end
end

describe "Marshal.load with nested modules" do
  it "loads" do
    obj = Custom::Nested.new
    data = Marshal.dump obj
    Marshal.load(data).should == obj
  end
end

class CustomWithIvar
  def _dump(depth)
    s = "stuff"
    s.instance_variable_set(:@foo, :CustomWithIvar)
    s
  end
  def self._load(s); s; end
end
class Custom
  def _dump(depth); "stuff"; end
  def self._load(s); s; end
end

describe "Marshal.load with serialized object having class name for _load method" do
  it "returns the object from _load" do
    obj = CustomWithIvar.new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should ==
      "#{mv+nv}I\"\x0Astuff\x06:\x09@foo:\x13CustomWithIvar"
  end
end

describe "Marshal.load with serializated extended_object having class name for _load method" do
  it "returns the object from _load" do
    obj = Custom.new.extend(Meths)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == "#{mv+nv}\"\x0Astuff"
  end
end

class OtherCustomWithIvar
  def marshal_dump
    a = []
    a.instance_variable_set(:@foo, :OtherCustomWithIvar)
    a
  end
  def marshal_load(o); o; end
end
class OtherCustom
  def marshal_dump; "stuff"; end
  def marshal_load(o); o; end
end

describe "Marshal.load with serialized object having class name for marshal_load method" do
  it "returns the object from marshal_load" do
    obj = OtherCustomWithIvar.new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_object having class name for marshal_load method" do
  it "returns the object from marshal_load" do
    obj = OtherCustom.new.extend(Meths)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

class UserObject
end

describe "Marshal.load with serialized object" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(Object.new))).should == Marshal.dump(Object.new)
  end
end

describe "Marshal.load with serialized user_object" do
  it "returns a construction of the argument" do
    obj = UserObject.new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_object" do
  it "returns a construction of the argument" do
    obj = Object.new.extend(Meths)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized object having ivar" do
  it "returns a construction of the argument" do
    s = 'hi'
    obj = Object.new
    obj.instance_variable_set(:@str, [:so, :so, s, s])
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized regexp" do
  it "returns a construction of the argument" do
    obj = /\A.\Z/
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_regexp" do
  it "returns a construction of the argument" do
    obj = /[a-z]/.extend(Meths, MethsMore)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

class UserRegexp < Regexp
end

describe "Marshal.load with serialized user_regexp having option Regexp::IGNORECASE" do
  it "returns a construction of the argument" do
    obj = UserRegexp.new('', Regexp::IGNORECASE)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_user_regexp having ivar" do
  it "returns a construction of the argument" do
    obj = UserRegexp.new('').extend(Meths)
    obj.instance_variable_set(:@noise, 'much')
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized float 0.0" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(0.0))).should == Marshal.dump(0.0)
  end
end

describe "Marshal.load with serialized float -0.0" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(-0.0))).should == Marshal.dump(-0.0)
  end
end

describe "Marshal.load with serialized float NaN" do
  it "returns a construction of the argument" do
    obj = 0.0 / 0.0
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized float Infinity" do
  it "returns a construction of the argument" do
    obj = 1.0 / 0.0
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized float -Infinity" do
  it "returns a construction of the argument" do
    obj = -1.0 / 0.0
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized float 1.0" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(1.0))).should == Marshal.dump(1.0)
  end
end

describe "Marshal.load with serialized float 1.3" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(1.3))).should == Marshal.dump(1.3)
  end
end

describe "Marshal.load with serialized float -5.1867345e-22" do
  it "returns a construction of the argument" do
    obj = -5.1867345e-22
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized float 1.1867345e+22" do
  it "returns a construction of the argument" do
    obj = 1.1867345e+22
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized hash" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(Hash.new))).should == Marshal.dump(Hash.new)
  end
end

describe "Marshal.load with serialized extended_hash" do
  it "returns a construction of the argument" do
    s = 'hi'
    obj = {'a' => :no, 'b' => s, 'c' => :no, 'd' => :no, 'e' => s, 'f' => :Meths}
    obj.extend(Meths, MethsMore)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

class UserHash < Hash
end

describe "Marshal.load with serialized user_hash" do
  it "returns a construction of the argument" do
    obj = UserHash.new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_user_hash_default" do
  it "returns a construction of the argument" do
    obj = UserHash.new(:Meths).extend(Meths)
    obj['three'] = 3
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized array" do
  it "returns a construction of the argument" do
    Marshal.dump(Marshal.load(Marshal.dump(Array.new))).should == Marshal.dump(Array.new)
  end
end

describe "Marshal.load with serialized array containing objects having _dump method, and with proc" do
  it "returns a construction of the argument" do
    arr = []
    proc = Proc.new { |o| arr << o }
    o1 = Custom.new; o2 = CustomWithIvar.new
    obj = [o1, o2, o1, o2]
    Marshal.dump(Marshal.load(Marshal.dump(obj), proc)).should ==
      "#{mv+nv}[\x09\"\x0AstuffI\"\x0Astuff\x06:\x09@foo:\x13CustomWithIvar@\x06@\x07"
    arr.length.should == 3
    arr[0].should == "stuff"
    arr[1].should == "stuff"
    arr[1].instance_variables.first.should == "@foo"
    arr[2].class.should == Array
    arr[2].length.should == 4
  end
end

describe "Marshal.load with serialized array containing objects having marshal_dump method, and with proc" do
  it "returns a construction of the argument" do
    arr = []
    proc = Proc.new { |o| arr << o }
    o1 = OtherCustom.new; o2 = OtherCustomWithIvar.new
    obj = [o1, o2, o1, o2]
    Marshal.dump(Marshal.load(Marshal.dump(obj), proc)).should == Marshal.dump(obj)
    arr.length.should == 5
    arr[0].should == "stuff"
    arr[1].class.should == OtherCustom
    arr[2].class.should == Array
    arr[3].class.should == OtherCustomWithIvar
    arr[4].class.should == Array
    arr[4].length.should == 4
  end
end

describe "Marshal.load with serialized array and proc" do
  it "returns a construction of the argument" do
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
    Marshal.load(Marshal.dump(obj), proc)
    arr.to_s.should ==
      [5, s, 10, 0.0, 'none', st, 'nine', 9, 'def', h, :b, :c, 2, a, 'ant', obj].to_s
  end
end

describe "Marshal.load with serialized array containing the same objects" do
  it "returns a construction of the argument" do
    s = 'oh'; b = 'hi'; r = //; d = [b, :no, s, :go]; c = String; f = 1.0
    o1 = OtherCustomWithIvar.new; o2 = OtherCustom.new
    obj = [:so, 'hello', 100, :so, :so, d, :so, o2, :so, :no, o2,
           :go, c, nil, Struct.new("Pyramid").new, f, :go, :no, s, b, r,
           :so, 'huh', o1, true, b, b, 99, r, b, s, :so, f, c, :no, o1, d]
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_array having ivar" do
  it "returns a construction of the argument" do
    s = 'well'
    s.instance_variable_set(:@foo, 10)
    obj = ['5', s, 'hi'].extend(Meths, MethsMore)
    obj.instance_variable_set(:@mix, s)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

class UserArray < Array
end

describe "Marshal.load with serialized user_array" do
  it "returns a construction of the argument" do
    obj = UserArray.new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_user_array" do
  it "returns a construction of the argument" do
    obj = UserArray.new.extend(Meths)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized struct" do
  it "returns a construction of the argument" do
    obj = Struct.new("Ure0").new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized struct having ivar" do
  it "returns a construction of the argument" do
    obj = Struct.new("Thick").new
    obj.instance_variable_set(:@foo, 5)
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized struct having fields" do
  it "returns a construction of the argument" do
    obj = Struct.new("Ure1", :a, :b).new
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end

describe "Marshal.load with serialized extended_struct having fields with same objects" do
  it "returns a construction of the argument" do
    s = 'hi'
    obj = Struct.new("Ure2", :a, :b).new.extend(Meths)
    obj.a = [:a, s]; obj.b = [:Meths, s]
    Marshal.dump(Marshal.load(Marshal.dump(obj))).should == Marshal.dump(obj)
  end
end
