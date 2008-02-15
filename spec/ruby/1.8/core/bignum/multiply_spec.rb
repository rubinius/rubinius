require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#*" do
  before(:each) do
    @bignum = bignum_value(772)
  end
  
  it "returns self multiplied by the given Integer" do
    (@bignum * (1/0xffff_ffff.to_f)).should be_close(2147483648.5, TOLERANCE)
    (@bignum * 10).to_s.should == "92233720368547765800"
    (@bignum * (@bignum - 40)).to_s.should == "85070591730234629737795195287525433200"
  end

  it "raises a TypeError when given a non-Integer" do
    lambda {
      (obj = mock('10')).should_receive(:to_int).any_number_of_times.and_return(10)
      @bignum * obj
    }.should raise_error(TypeError)
    lambda { @bignum * "10"    }.should raise_error(TypeError)
    lambda { @bignum * :symbol }.should raise_error(TypeError)
  end
end
