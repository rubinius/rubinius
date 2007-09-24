require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#*" do
  it "returns self multiplied by the given Integer" do
    (4923 * 2).should == 9846
    (1342177 * 800).should == 1073741600
    (65536 * 65536).should == 4294967296

    (256 * 0xffffffff).should == 1099511627520
    (6712 * 0.25).should == 1678.0
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError, "Object can't be coerced into Fixnum") do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13 * obj
    end
    
    should_raise(TypeError, "String can't be coerced into Fixnum") do
      13 * "10"
    end
    
    should_raise(TypeError, ":symbol can't be coerced into Fixnum") do
      13 * :symbol
    end
  end
end
