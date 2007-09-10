require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#to_s" do  
  it "returns a string representation of self" do
    BignumHelper.sbm(9).to_s.should == "1073741833"
    BignumHelper.sbm.to_s.should == "1073741824"
    (-BignumHelper.sbm(675)).to_s.should == "-1073742499"
  end
  
  it "returns a string with the representation of self in base x"  do 
    a = 18446744073709551616 #2**64
    a.to_s(2).should == "10000000000000000000000000000000000000000000000000000000000000000" 
    a.to_s(8).should == "2000000000000000000000"
    a.to_s(16).should == "10000000000000000"
    a.to_s(32).should == "g000000000000" 
  end
  
  it "raises an ArgumentError exception if argument is 0" do
    should_raise(ArgumentError){ 18446744073709551616.to_s(0) }
  end
  
  it "raises an ArgumentError exception if argument is bigger than 36" do 
    should_raise(ArgumentError){ 18446744073709551616.to_s(37) } # Max is 36
  end
end
