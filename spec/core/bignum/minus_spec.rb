require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#-" do
  before(:each) do
    @bignum = BignumHelper.sbm(314)
  end
  
  it "returns self minus the given Integer" do
    (@bignum - 9).should == 1073742129
    (@bignum - 12.57).should == 1073742125.43
    (@bignum - BignumHelper.sbm(42)).should == 272
  end

  it "raises a TypeError when given a non-Integer" do
    lambda {
      (obj = Object.new).should_receive(:to_int).and_return(10)
      @bignum - obj
    }.should raise_error(TypeError)
    lambda { @bignum - "10" }.should raise_error(TypeError)
    lambda { @bignum - :symbol }.should raise_error(TypeError)
  end
end
