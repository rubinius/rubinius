require File.expand_path('../../../spec_helper', __FILE__)

describe "Bignum#==" do
  before(:each) do
    @bignum = bignum_value
  end

  it "returns true if self has the same value as the given argument" do
    (@bignum == @bignum).should == true
    (@bignum == @bignum.to_f).should == true

    (@bignum == @bignum + 1).should == false
    (@bignum + 1 == @bignum).should == false

    (@bignum == 9).should == false
    (@bignum == 9.01).should == false

    (@bignum == bignum_value(10)).should == false
  end

  it "calls 'other == self' if the given argument is not an Integer" do
    obj = mock('not integer')
    obj.should_receive(:==).and_return(true)
    (@bignum == obj).should == true
  end

  it "returns the result of 'other == self' as a boolean" do
    obj = mock('not integer')
    obj.should_receive(:==).exactly(2).times.and_return("woot", nil)
    (@bignum == obj).should == true
    (@bignum == obj).should == false
  end
end
