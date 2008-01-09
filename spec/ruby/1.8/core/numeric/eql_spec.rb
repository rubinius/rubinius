require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#eql?" do 
  before(:each) do 
    @integer  = 1
    @float    = 1.0
    @bignum   = 4294967296
    @bigfloat = 4294967296.0
    @numeric = Numeric.new
  end

  after(:each) do 
    @integer  = nil
    @float    = nil
    @bignum   = nil
    @bigfloat = nil
    @numeric = nil
  end
  
  it "be equal (integers and floats)" do 
    @integer.eql?(@integer).should == true
    @integer.eql?(@float).should == false
    @float.eql?(@float).should == true
    @float.eql?(@integer).should == false
  end
  
  it " should be equal (bignums and floats " do
    @bignum.eql?(@bignum).should == true 
    @bignum.eql?(@bigfloat).should == false
  end
  
  it "be equal (edge cases)" do  
    0.eql?(0).should ==  true
    1.0.eql?(1.00000000000000000000).should == true
    0.eql?(0.0).should ==  false
    000000.eql?(0.0).should ==  false
    000000.eql?(000) .should ==  true
    @numeric.eql?(@numeric).should == true
    @numeric.eql?(Numeric.new).should == false
  end
end
