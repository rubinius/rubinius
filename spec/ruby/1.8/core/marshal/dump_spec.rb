require File.dirname(__FILE__) + '/../../spec_helper'

mv = "\x04"  # Marshal::MAJOR_VERSION
nv = "\x08"  # Marshal::MINOR_VERSION

describe "Marshal.dump when given a recursion limit" do
  it "raises an ArgumentError when the recursion limit is exceeded" do
    Marshal.dump([], 1)
    Marshal.dump([[]], 2)
    Marshal.dump([[[]]], 3)

    h = {'one' => {'two' => {'three' => 0}}}
    lambda { Marshal.dump(h, 3) }.should raise_error(ArgumentError)
    lambda { Marshal.dump([h], 4) }.should raise_error(ArgumentError)
    Marshal.dump({}, 1)
    Marshal.dump(h, 4)

    lambda { Marshal.dump([], 0)     }.should raise_error(ArgumentError)
    lambda { Marshal.dump([[[]]], 1) }.should raise_error(ArgumentError)
  end

  it "ignores the recursion limit if the limit is negative" do
    Marshal.dump([], -1)
    Marshal.dump([[]], -1)
    Marshal.dump([[[]]], -1)
  end
end

describe "Marshal.dump when given an IO-Object" do
  it "writes the serialized data to the IO-Object" do
    (obj = mock('test')).should_receive(:write).at_least(1)
    Marshal.dump("test", obj)
  end

  it "returns the IO-Object" do
    (obj = mock('test')).should_receive(:write).at_least(1)
    Marshal.dump("test", obj).should == obj
  end
  
  it "raises an Error when the IO-Object does not respond to #write" do
    obj = mock('test')
    lambda { Marshal.dump("test", obj) }.should raise_error(TypeError)
  end
end

describe "Marshal.dump" do
  it "is provided" do
    Marshal.respond_to?(:dump).should == true
  end
  
  it "returns a string-serialized version of the given argument" do
    Marshal.dump("test").is_a?(String).should == true
  end

  it "raises an Error when trying to dump an anonymous class/module" do
    klass = Class.new
    mod = Module.new
    
    lambda { Marshal.dump(klass) }.should raise_error(TypeError)
    lambda { Marshal.dump(mod)   }.should raise_error(TypeError)
  end
end

describe "Marshal.dump with nil" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(nil).should == "#{mv+nv}0"
  end
end

describe "Marshal.dump with true" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(true).should == "#{mv+nv}T"
  end
end

describe "Marshal.dump with false" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(false).should == "#{mv+nv}F"
  end
end

describe "Marshal.dump with empty string" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump('').should == "#{mv+nv}\"\x00"
  end
end

describe "Marshal.dump with string of length 100" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump('a' * 100).should == "#{mv+nv}\"\x69" + ('a' * 100)
  end
end

describe "Marshal.dump with string of length 200" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump('a' * 200).should == "#{mv+nv}\"\x01\xC8" + ('a' * 200)
  end
end

describe "Marshal.dump with string of length 300" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump('a' * 300).should == "#{mv+nv}\"\x02\x2C\x01" + ('a' * 300)
  end
end

module Meths
end
module MethsMore
end

describe "Marshal.dump with empty extended_string" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(''.extend(Meths)).should == "#{mv+nv}e:\x0AMeths\"\x00"
  end
end

class UserString < String
end

describe "Marshal.dump with empty user_string" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserString.new).should == "#{mv+nv}C:\x0FUserString\"\x00"
  end
end

describe "Marshal.dump with empty extended_user_string" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserString.new.extend(Meths)).should == "#{mv+nv}e:\x0AMethsC:\x0FUserString\"\x00"
  end
end

describe "Marshal.dump with symbol of length 1" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(:a).should == "#{mv+nv}:\x06a"
  end
end

describe "Marshal.dump with symbol of length 300" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(('a' * 300).to_sym).should == "#{mv+nv}:\x02\x2C\x01" + ('a' * 300).to_sym.to_s
  end
end

describe "Marshal.dump with fixnum 0" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(0).should == "#{mv+nv}i\x00"
  end
end

describe "Marshal.dump with fixnum 5" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(5).should == "#{mv+nv}i\x0A"
  end
end

describe "Marshal.dump with fixnum -123" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-123).should == "#{mv+nv}i\x80"
  end
end

describe "Marshal.dump with fixnum 2**8" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**8).should == "#{mv+nv}i\x02\x00\x01"
  end
end

describe "Marshal.dump with fixnum -2**8" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**8).should == "#{mv+nv}i\xFF\x00"
  end
end

describe "Marshal.dump with fixnum 2**24" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**24).should == "#{mv+nv}i\x04\x00\x00\x00\x01"
  end
end

describe "Marshal.dump with fixnum (2**30 - 1)" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**30 - 1).should == "#{mv+nv}i\x04\xFF\xFF\xFF\x3F"
  end
end

describe "Marshal.dump with fixnum -16711680" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-16711680).should == "#{mv+nv}i\xFD\x00\x00\x01"
  end
end

describe "Marshal.dump with fixnum 2**16" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**16).should == "#{mv+nv}i\x03\x00\x00\x01"
  end
end

describe "Marshal.dump with fixnum -2**16" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**16).should == "#{mv+nv}i\xFE\x00\x00"
  end
end

describe "Marshal.dump with fixnum -(2**16 - 1)" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-(2**16 - 1)).should == "#{mv+nv}i\xFE\x01\x00"
  end
end

describe "Marshal.dump with fixnum 43690" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(43690).should == "#{mv+nv}i\x02\xAA\xAA"
  end
end

describe "Marshal.dump with fixnum -43690" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-43690).should == "#{mv+nv}i\xFE\x56\x55"
  end
end

describe "Marshal.dump with fixnum -2**30" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**30).should == "#{mv+nv}i\xFC\x00\x00\x00\xC0"
  end
end

describe "Marshal.dump with bignum 2**30" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**30).should == "#{mv+nv}l+\x07\x00\x00\x00\x40"
  end
end

describe "Marshal.dump with bignum -(2**31 - 1)" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-(2**31 - 1)).should == "#{mv+nv}l-\x07\xFF\xFF\xFF\x7F"
  end
end

describe "Marshal.dump with bignum 2**40" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**40).should == "#{mv+nv}l+\x08\x00\x00\x00\x00\x00\x01"
  end
end

describe "Marshal.dump with bignum -2**63" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**63).should == "#{mv+nv}l-\x09\x00\x00\x00\x00\x00\x00\x00\x80"
  end
end

describe "Marshal.dump with bignum 2**64" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**64).should == "#{mv+nv}l+\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00"
  end
end

describe "Marshal.dump with bignum -2**64" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**64).should == "#{mv+nv}l-\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00"
  end
end

describe "Marshal.dump with bignum 2**90" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**90).should == "#{mv+nv}l+\x0B\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04"
  end
end

describe "Marshal.dump with class" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(String).should == "#{mv+nv}c\x0BString"
  end
end

describe "Marshal.dump with module" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Marshal).should == "#{mv+nv}m\x0CMarshal"
  end
end

class CustomWithIvar
  def _dump(depth)
    s = "stuff"
    s.instance_variable_set(:@foo, 5)
    s
  end
end
class Custom
  def _dump(depth); "stuff"; end
end
class BadCustom
  def _dump(depth); 10; end
end

describe "Marshal.dump with any object having _dump method returning non-string" do
  it "raises a TypeError" do
    lambda { Marshal.dump(BadCustom.new) }.should raise_error(TypeError)
  end
end

describe "Marshal.dump with any object having _dump method" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(CustomWithIvar.new).should == "#{mv+nv}Iu:\x13CustomWithIvar\x0Astuff\x06:\x09@fooi\x0A"
  end
end

describe "Marshal.dump with any extended_object having _dump method" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Custom.new.extend(Meths)).should == "#{mv+nv}u:\x0BCustom\x0Astuff"
  end
end

class OtherCustomWithIvar
  def marshal_dump
    a = []
    a.instance_variable_set(:@foo, 'hi')
    a
  end
end
class OtherCustom
  def marshal_dump; "stuff"; end
end

describe "Marshal.dump with any object having marshal_dump method" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(OtherCustomWithIvar.new).should ==
      "#{mv+nv}U:\x18OtherCustomWithIvarI[\x00\x06:\x09@foo\"\x07hi"
  end
end

describe "Marshal.dump with any extended_object having marshal_dump method" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(OtherCustom.new.extend(Meths)).should == "#{mv+nv}U:\x10OtherCustom\"\x0Astuff"
  end
end

class UserObject
end

describe "Marshal.dump with object" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Object.new).should == "#{mv+nv}o:\x0BObject\x00"
  end
end

describe "Marshal.dump with user_object" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserObject.new).should == "#{mv+nv}o:\x0FUserObject\x00"
  end
end

describe "Marshal.dump with extended_object" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Object.new.extend(Meths)).should == "#{mv+nv}e:\x0AMethso:\x0BObject\x00"
  end
end

describe "Marshal.dump with object having ivar" do
  it "returns a string-serialized version of the given argument" do
    s = 'hi'
    obj = Object.new
    obj.instance_variable_set(:@str, [:so, :so, s, s])
    Marshal.dump(obj).should == "#{mv+nv}o:\x0BObject\x06:\x09@str[\x09:\x07so;\x07\"\x07hi@\x07"
  end
end

describe "Marshal.dump with regexp" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(/\A.\Z/).should == "#{mv+nv}/\x0A\\A.\\Z\x00"
  end
end

describe "Marshal.dump with extended_regexp" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(/[a-z]/.extend(Meths, MethsMore)).should ==
      "#{mv+nv}e:\x0AMethse:\x0EMethsMore/\x0A[a-z]\x00"
  end
end

class UserRegexp < Regexp
end

describe "Marshal.dump with user_regexp having option Regexp::IGNORECASE" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserRegexp.new('', Regexp::IGNORECASE)).should ==
      "#{mv+nv}C:\x0FUserRegexp/\x00\x01"
  end
end

describe "Marshal.dump with extended_user_regexp having ivar" do
  it "returns a string-serialized version of the given argument" do
    r = UserRegexp.new('').extend(Meths)
    r.instance_variable_set(:@noise, 'much')
    Marshal.dump(r).should == "#{mv+nv}Ie:\x0AMethsC:\x0FUserRegexp/\x00\x00\x06:\x0B@noise\"\x09much"
  end
end

describe "Marshal.dump with float 0.0" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(0.0).should == "#{mv+nv}f\x060"
  end
end

describe "Marshal.dump with float -0.0" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-0.0).should == "#{mv+nv}f\x07-0"
  end
end

describe "Marshal.dump with float NaN" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(0.0 / 0.0).should == "#{mv+nv}f\x08nan"
  end
end

describe "Marshal.dump with float Infinity" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(1.0 / 0.0).should == "#{mv+nv}f\x08inf"
  end
end

describe "Marshal.dump with float -Infinity" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-1.0 / 0.0).should == "#{mv+nv}f\x09-inf"
  end
end

describe "Marshal.dump with float 1.0" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(1.0).should == "#{mv+nv}f\x061"
  end
end

describe "Marshal.dump with float 1.3" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(1.3).should == "#{mv+nv}f\x0B1.3\x00\xCC\xCD"
  end
end

describe "Marshal.dump with float -5.1867345e-22" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-5.1867345e-22).should == "#{mv+nv}f\x1F-5.1867344999999998e-22\x00\x83\x5E"
  end
end

describe "Marshal.dump with float 1.1867345e+22" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(1.1867345e+22).should == "#{mv+nv}f\x1E1.1867344999999999e+22\x00\xE4\x40"
  end
end

describe "Marshal.dump with hash" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Hash.new).should == "#{mv+nv}{\x00"
  end
end

describe "Marshal.dump with hash having default proc" do
  it "raises a TypeError" do
    lambda { Marshal.dump(Hash.new {}) }.should raise_error(TypeError)
  end
end

describe "Marshal.dump with extended_hash" do
  it "returns a string-serialized version of the given argument" do
    s = 'hi'
    h = {'a' => :no, 'b' => s, 'c' => :no, 'd' => :no, 'e' => s, 'f' => :Meths}
    Marshal.dump(h.extend(Meths, MethsMore)).should ==
      "#{mv+nv}e:\x0AMethse:\x0EMethsMore{\x0B\"\x06a:\x07no\"\x06b\"\x07hi\"\x06c;\x07\"\x06d;\x07\"\x06e@\x08\"\x06f;\x00"
  end
end

class UserHash < Hash
end

describe "Marshal.dump with user_hash" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserHash.new).should == "#{mv+nv}C:\x0DUserHash{\x00"
  end
end

describe "Marshal.dump with extended_user_hash_default" do
  it "returns a string-serialized version of the given argument" do
    h = UserHash.new(:Meths).extend(Meths)
    h['three'] = 3
    Marshal.dump(h).should == "#{mv+nv}e:\x0AMethsC:\x0DUserHash}\x06\"\x0Athreei\x08;\x00"
  end
end

describe "Marshal.dump with array" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Array.new).should == "#{mv+nv}[\x00"
  end
end

describe "Marshal.dump with array containing the same objects" do
  it "returns a string-serialized version of the given argument" do
    s = 'oh'; b = 'hi'; r = //; d = [b, :no, s, :go]; c = String
    a = [:so, 'hello', 100, :so, :so, d, :so, :so, :no, :go, c, nil,
          :go, :no, s, b, r, :so, 'huh', true, b, b, 99, r, b, s, :so, c, :no, d]
    Marshal.dump(a).should ==
      "#{mv+nv}[\x23:\x07so\"\x0Ahelloi\x69;\x00;\x00[\x09\"\x07hi:\x07no\"\x07oh:\x07go;\x00;\x00;\x06;\x07c\x0BString0;\x07;\x06@\x09@\x08/\x00\x00;\x00\"\x08huhT@\x08@\x08i\x68@\x0B@\x08@\x09;\x00@\x0A;\x06@\x07"
  end
end

describe "Marshal.dump with extended_array having ivar" do
  it "returns a string-serialized version of the given argument" do
    s = 'well'
    s.instance_variable_set(:@foo, 10)
    a = ['5', s, 'hi'].extend(Meths, MethsMore)
    a.instance_variable_set(:@mix, s)
    Marshal.dump(a).should ==
      "#{mv+nv}Ie:\x0AMethse:\x0EMethsMore[\x08\"\x065I\"\x09well\x06:\x09@fooi\x0F\"\x07hi\x06:\x09@mix@\x07"
  end
end

class UserArray < Array
end

describe "Marshal.dump with user_array" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserArray.new).should == "#{mv+nv}C:\x0EUserArray[\x00"
  end
end

describe "Marshal.dump with extended_user_array" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserArray.new.extend(Meths)).should ==
      "#{mv+nv}e:\x0AMethsC:\x0EUserArray[\x00"
  end
end

describe "Marshal.dump with struct" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Struct.new("Ure0").new).should == "#{mv+nv}S:\x11Struct::Ure0\x00"
  end
end

describe "Marshal.dump with struct having fields" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Struct.new("Ure1", :a, :b).new).should == "#{mv+nv}S:\x11Struct::Ure1\x07:\x06a0:\x06b0"
  end
end

describe "Marshal.dump with extended_struct having fields with same objects" do
  it "returns a string-serialized version of the given argument" do
    s = 'hi'
    st = Struct.new("Ure2", :a, :b).new.extend(Meths)
    st.a = [:a, s]; st.b = [:Meths, s]
    Marshal.dump(st).should ==
      "#{mv+nv}e:\x0AMethsS:\x11Struct::Ure2\x07:\x06a[\x07;\x07\"\x07hi:\x06b[\x07;\x00@\x07"
  end
end
