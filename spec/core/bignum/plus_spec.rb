require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#+" do
  before(:each) do
    @bignum = BignumHelper.sbm(76)
  end
  
  it "returns self plus the given Integer" do
    (@bignum + 4).should == 1073741904
    (@bignum + 4.2).should == 1073741904.2
    (@bignum + BignumHelper.sbm(3)).should == 2147483727
  end

  it "raises a TypeError when given a non-Integer" do
    lambda {
      (obj = mock('10')).should_receive(:to_int).any_number_of_times.and_return(10)
      @bignum + obj
    }.should raise_error(TypeError)
    lambda { @bignum + "10" }.should raise_error(TypeError)
    lambda { @bignum + :symbol}.should raise_error(TypeError)
  end
end
