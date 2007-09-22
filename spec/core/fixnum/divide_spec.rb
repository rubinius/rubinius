require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#/" do
  it "returns self divided by the given argument" do
    (2 / 2).should == 1
    (3 / 2).should == 1
  end
  
  it "raises ZeroDivisionError if the given argument is zero and not a Float" do
    should_raise(ZeroDivisionError, "divided by 0") { 1 / 0 }
  end
  
  it "does NOT raise ZeroDivisionError if the given argument is zero and is a Float" do
    (1 / 0.0).to_s.should == 'Infinity'
    (-1 / 0.0).to_s.should == '-Infinity'
  end

  it "coerces fixnum and return self divided by other" do
    (-1 / 50.4).to_s.should == "-0.0198412698412698"
    (1 / 0xffffffff).should == 0
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError, "Object can't be coerced into Fixnum") do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13 / obj
    end
    
    should_raise(TypeError, "String can't be coerced into Fixnum") do
      13 / "10"
    end
  end
end
