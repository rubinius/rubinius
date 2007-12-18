require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#|" do
  before(:each) do
    @bignum = BignumHelper.sbm(11) 
  end
  
  it "returns self bitwise OR other" do
    (@bignum | 2).should == 1073741835
    (@bignum | 9).should == 1073741835
    (@bignum | BignumHelper.sbm).should == 1073741835
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (@bignum | 9.9).should == 1073741835
    
    (obj = mock('2')).should_receive(:to_int).and_return(2)
    (@bignum | obj).should == 1073741835
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum | obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum | obj }.should raise_error(TypeError)
  end
end
