require File.dirname(__FILE__) + '/../../spec_helper'

# Returns an array containing the quotient and modulus obtained
# by dividing num by aNumeric. If q, r = x.divmod(y), then
#    q = floor(float(x)/float(y))
#    x = q*y + r
describe "Numeric#divmod" do    
  it "divmod right  integers" do 
    13.divmod(a_den_int).should == [3,1]
    a_den_int.divmod(13).should == [0,4]
  end
  
  it "divmod right  integers and floats" do 
    13.divmod(4.0).should == [3,1]
    a_den_int.divmod(13).should == [0,4]
  end
  
  it "divmod right the integers and floats" do 
    13.divmod(4.0).should == [3,1]
    a_den_int.divmod(13).should == [0,4]
  end
  
  it "divmod right  floats" do 
    13.0.divmod(4.0).should == [3.0,1.0]
    4.0.divmod(13).should == [0.0,4.0]
  end
  
  it " should divmod right with bignums and integers" do
    3**33.divmod( 100).should == [55590605665555, 23]
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ 13.divmod }
    should_raise(ZeroDivisionError){ 13.divmod(0) }
    should_raise(TypeError){ 13.divmod(nil) }
    should_raise(TypeError){ 13.divmod('test') }
    should_raise(TypeError){ 13.divmod(true) }   
  end 
end
