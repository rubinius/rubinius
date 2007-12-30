require File.dirname(__FILE__) + '/../../spec_helper'

mv = "\x04"  # Marshal::MAJOR_VERSION
nv = "\x08"  # Marshal::MINOR_VERSION

describe "Marshal.dump when given a recursion limit" do
  it "raises an ArgumentError when the recursion limit is exceeded" do
    Marshal.dump([], 1)
    Marshal.dump([[]], 2)
    Marshal.dump([[[]]], 3)
    
    lambda { Marshal.dump([], 0)     }.should raise_error(ArgumentError)
    lambda { Marshal.dump([[[]]], 1) }.should raise_error(ArgumentError)
  end
  
  it "ignores the recursion limit if the limit is negative" do
    fails_on :rubinius do
      Marshal.dump([], -1)
      Marshal.dump([[]], -1)
      Marshal.dump([[[]]], -1)
    end
  end
end

describe "Marshal.dump when given an IO-Object" do
  it "should write the serialized data to the IO-Object" do
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
  it "should be provided" do
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
    Marshal.dump(''.extend Meths).should == "#{mv+nv}e:\x0AMeths\"\x00"
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
    Marshal.dump(UserString.new.extend Meths).should == "#{mv+nv}e:\x0AMethsC:\x0FUserString\"\x00"
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

describe "Marshal.dump with fixnum 2**16" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(2**16).should == "#{mv+nv}i\x03\x00\x00\x01"
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

describe "Marshal.dump with fixnum -2**16" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**16).should == "#{mv+nv}i\xFE\x00\x00"
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

describe "Marshal.dump with bignum -2**31" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-2**31).should == "#{mv+nv}l-\x07\x00\x00\x00\x80"
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

class Custom
  def _dump(depth); "stuff"; end
end

describe "Marshal.dump with any object having _dump method" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Custom.new).should == "#{mv+nv}u:\x0BCustom\x0Astuff"
  end
end

describe "Marshal.dump with object" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Object.new).should == "#{mv+nv}o:\x0BObject\x00"
  end
end

describe "Marshal.dump with object having ivar" do
  it "returns a string-serialized version of the given argument" do
    obj = Object.new
    obj.instance_variable_set(:@str, 'hi')
    Marshal.dump(obj).should == "#{mv+nv}o:\x0BObject\x06:\x09@str\"\x07hi"
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

describe "Marshal.dump with extended_user_regexp" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(UserRegexp.new('').extend Meths).should ==
      "#{mv+nv}e:\x0AMethsC:\x0FUserRegexp/\x00\x00"
  end
end

describe "Marshal.dump with float 0.0" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(0.0).should == "#{mv+nv}f\x060"
  end
end

describe "Marshal.dump with float NaN" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(0.0 / 0.0).should == "#{mv+nv}f\x08nan"
  end
end

describe "Marshal.dump with float -Infinity" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(-1.0 / 0.0).should == "#{mv+nv}f\x09-inf"
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

describe "Marshal.dump with hash" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Hash.new).should == "#{mv+nv}{\x00"
  end
end

describe "Marshal.dump with extended_hash" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump({'one' => 1}.extend(Meths, MethsMore)).should ==
      "#{mv+nv}e:\x0AMethse:\x0EMethsMore{\x06\"\x08onei\x06"
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
    h = UserHash.new('default').extend(Meths)
    h['three'] = 3
    Marshal.dump(h).should == "#{mv+nv}e:\x0AMethsC:\x0DUserHash}\x06\"\x0Athreei\x08\"\x0Cdefault"
  end
end

describe "Marshal.dump with array" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Array.new).should == "#{mv+nv}[\x00"
  end
end

describe "Marshal.dump with array containing the same symbols" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump(Array.new(5, :so)).should == "#{mv+nv}[\x0A:\x07so;\x00;\x00;\x00;\x00"
  end
end

describe "Marshal.dump with array containing refs to the same object" do
  it "returns a string-serialized version of the given argument" do
    s = "hi"; n = 5; b = 2**64
    Marshal.dump([n, n, s, s, s, b, b, b]).should ==
      "#{mv+nv}[\x0Di\x0Ai\x0A\"\x07hi@\x06@\x06l+\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00@\x07@\x07"
  end
end

describe "Marshal.dump with extended_array" do
  it "returns a string-serialized version of the given argument" do
    Marshal.dump([5, 'hi'].extend(Meths, MethsMore)).should ==
      "#{mv+nv}e:\x0AMethse:\x0EMethsMore[\x07i\x0A\"\x07hi"
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
    Marshal.dump(UserArray.new.extend Meths).should ==
      "#{mv+nv}e:\x0AMethsC:\x0EUserArray[\x00"
  end
end
