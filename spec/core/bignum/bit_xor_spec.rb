require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#^" do
  before(:each) do
    @bignum = BignumHelper.sbm(18) 
  end
  
  it "returns self bitwise EXCLUSIVE OR other" do
    (@bignum ^ 2).should == 1073741840
    (@bignum ^ @bignum).should == 0
    (@bignum ^ 14).should == 1073741852
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (@bignum ^ 14.5).should == 1073741852
    
    (obj = Object.new).should_receive(:to_int).and_return(2)
    (@bignum ^ obj).should == 1073741840
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = Object.new
    lambda { @bignum ^ obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum ^ obj }.should raise_error(TypeError)
  end
end
