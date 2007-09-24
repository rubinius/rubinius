require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#+" do
  it "returns self plus the given Integer" do
    (491 + 2).should == 493
    (90210 + 10).should == 90220

    (9 + 0xffffffff).should == 4294967304
    (1001 + 5.219).should == 1006.219
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError, "Object can't be coerced into Fixnum") do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13 + obj
    end
    
    should_raise(TypeError, "String can't be coerced into Fixnum") do
      13 + "10"
    end
    
    should_raise(TypeError, ":symbol can't be coerced into Fixnum") do
      13 + :symbol
    end
  end
end
