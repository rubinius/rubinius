require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#==" do
  it "should return true if self has the same value as other" do
    (1 == 1).should == true
    (9 == 5).should == false
    
    # Actually, these call Float#==, Bignum#== etc.
    (9 == 9.0).should == true
    (9 == 9.01).should == false
    
    (10 == 0xffffffff).should == false
  end
  
  it "calls 'other == self' if the given argument is no Fixnum" do
    (obj = Object.new).should_receive(:==).and_return(false)    
    (1 == obj).should == false
    
    obj.should_receive(:==).and_return(true)
    (2 == obj).should == true
  end
end
