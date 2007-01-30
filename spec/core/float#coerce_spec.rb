require File.dirname(__FILE__) + '/../spec_helper'

context "Float#coerce" do
  specify "should have a receiver that is a Float" do
    example { FLOAT.class }.should == Float
  end

  specify "should return Floats when a Fixnum is received" do
    x, y = example { FLOAT.coerce(FIXNUM) }
    x.class.should == Float
    x.class.should == y.class
  end

  specify "should return Floats when a Bignum is received" do
    x, y = example { FLOAT.coerce(BIGNUM) }
    x.class.should == Float
    x.class.should == y.class
  end

  specify "should return Floats when a Float is received" do
    x, y = example { FLOAT.coerce(FLOAT) }
    x.class.should == Float
    x.class.should == y.class
  end

  specify "should return correct arguments in the order: other, self" do
    example { FLOAT.coerce(FIXNUM) }.should == [FIXNUM.to_f, FLOAT]
    example { FLOAT.coerce(BIGNUM) }.should == [BIGNUM.to_f, FLOAT]
    example { FLOAT.coerce(FLOAT2) }.should == [FLOAT2, FLOAT]
  end
end

