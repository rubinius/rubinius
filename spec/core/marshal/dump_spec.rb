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
