require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#*" do
  before(:each) do
    @bignum = BignumHelper.sbm(772)
  end
  
  it "returns self multiplied by the given Integer" do
    (@bignum * 98.6).should_be_close(1.0587101996559999e+11, TOLERANCE)
    (@bignum * 10).to_s.should == '10737425960'
    (@bignum * (@bignum - 40)).to_s.should == '1152923119515115376'
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError) do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      @bignum * obj
    end
    
    should_raise(TypeError) do
      @bignum * "10"
    end
    
    should_raise(TypeError) do
      @bignum * :symbol
    end
  end
end
