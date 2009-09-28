require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#^" do
  before(:each) do
    @bignum = bignum_value(18) 
  end
  
  it "returns self bitwise EXCLUSIVE OR other" do
    (@bignum ^ 2).should == 9223372036854775824
    (@bignum ^ @bignum).should == 0
    (@bignum ^ 14).should == 9223372036854775836
  end

  ruby_version_is ""..."1.9" do
    it "coerces Float arguments into Integers" do
      (@bignum ^ 14.5).should == 9223372036854775836
      (bignum_value ^ bignum_value(0xffff).to_f).should == 65536
    end
  end

  ruby_version_is "1.9" do
    it "raises a TypeError when passed a Float" do
      lambda { @bignum ^ 14.5 }.should raise_error(TypeError)
      lambda { bignum_value ^ bignum_value(0xffff).to_f }.should 
        raise_error(TypeError)
    end
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (obj = mock('2')).should_receive(:to_int).and_return(2)
    (@bignum ^ obj).should == 9223372036854775824
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum ^ obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum ^ obj }.should raise_error(TypeError)
  end
end
