require File.dirname(__FILE__) + '/../spec_helper'

context "Fixnum#coerce" do
  specify "should have a receiver that is a Fixnum" do
    example { FIXNUM.class }.should == Fixnum
  end

  specify "should return Fixnums when a Fixnum is received" do
    x, y = example { FIXNUM.coerce(FIXNUM) }
    x.class.should == Fixnum
    x.class.should == y.class
  end

  specify "should return Bignums when a Bignum is received" do
    x, y = example { FIXNUM.coerce(BIGNUM) }
    x.class.should == Bignum
    x.class.should == y.class
  end

  specify "should return Floats when a Float is received" do
    x, y = example { FIXNUM.coerce(FLOAT) }
    x.class.should == Float
    x.class.should == y.class
  end

  specify "should return correct arguments in the order: other, self" do
    example { FIXNUM.coerce(FIXNUM2) }.should == [FIXNUM2, FIXNUM]
    example { FIXNUM.coerce(BIGNUM)  }.should == [BIGNUM, FIXNUM]
    example { FIXNUM.coerce(FLOAT)   }.should == [FLOAT, FIXNUM.to_f]
  end
end

