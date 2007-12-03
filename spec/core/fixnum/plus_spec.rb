require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#+" do
  it "returns self plus the given Integer" do
    (491 + 2).should == 493
    (90210 + 10).should == 90220

    (9 + 0xffffffff).should == 4294967304
    (1001 + 5.219).should == 1006.219
  end

  it "raises a TypeError when given a non-Integer" do
    lambda {
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13 + obj
    }.should raise_error(TypeError)
    lambda { 13 + "10"    }.should raise_error(TypeError)
    lambda { 13 + :symbol }.should raise_error(TypeError)
  end
end
