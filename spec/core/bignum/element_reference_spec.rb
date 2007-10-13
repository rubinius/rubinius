require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#[]" do
  before(:each) do
    @bignum = BignumHelper.sbm(4996)
  end
  
  it "returns the nth bit in the binary representation of self" do
    @bignum[2].should == 1
    @bignum[9.2].should == 1
    @bignum[21].should == 0
  end

  it "tries to convert the given argument to an Integer using #to_int" do
    @bignum[1.3].should == @bignum[1]
    
    (obj = Object.new).should_receive(:to_int, :returning => 2)
    @bignum[obj].should == 1
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = Object.new
    should_raise(TypeError) { @bignum[obj] }
    
    obj.should_receive(:to_int, :returning => "asdf")
    should_raise(TypeError) { @bignum[obj] }
  end
end
