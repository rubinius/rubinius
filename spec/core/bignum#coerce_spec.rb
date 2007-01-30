require File.dirname(__FILE__) + '/../spec_helper'

context "Bignum#coerce" do
  specify "should have a receiver that is a Bignum" do
    example { BIGNUM.class }.should == Bignum
  end

  specify "should return Bignums when a Fixnum is received" do
    x, y = example { BIGNUM.coerce(FIXNUM) }
    x.class.should == Bignum
    x.class.should == y.class
  end

  specify "should return Bignums when a Bignum is received" do
    x, y = example { BIGNUM.coerce(BIGNUM) }
    x.class.should == Bignum
    x.class.should == y.class
  end

  specify "should return Floats when a Float is received" do
    x, y = example { BIGNUM.coerce(FLOAT) }
    x.class.should == Float
    x.class.should == y.class
  end

  specify "should return correct arguments in the order: other, self" do
    example { BIGNUM.coerce(FIXNUM)  }.should == [FIXNUM, BIGNUM]
    example { BIGNUM.coerce(BIGNUM2) }.should == [BIGNUM2, BIGNUM]
    example { BIGNUM.coerce(FLOAT)   }.should == [FLOAT, BIGNUM.to_f]
  end
end

