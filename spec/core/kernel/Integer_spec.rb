require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.Integer when given a String" do
  it "does not call #to_i on the given String" do
    (obj = "2").should_not_receive(:to_i)
    Integer(obj).should == 2
  end
  
  it "ignores whitespaces" do
    Integer("  2  ").should == 2
    Integer("  22222  ").should == 22222
  end
  
  it "raises an ArgumentError if the given String is no valid String representation" do
    [ "--2", "-+2", "++2", "a2", "2a", "__2", " _2", "2__", "2 _", "2 a"].each do |str|
      should_raise(ArgumentError) do
        Integer(str)
      end
    end
  end
end

describe "Kernel.Integer" do
  it "calls #to_int if the given obejct responds to it" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 1)
    obj.should_not_receive(:to_i)
    
    Integer(obj).should == 1
  end
  
  it "should call to_i to convert any arbitrary argument to an Integer" do
    (obj = Object.new).should_receive(:to_i, :returning => 7)
    Integer(obj).should == 7
  end

  it "should raise a TypeError if there is no to_i method on an object" do
    should_raise(TypeError) { Integer(Object.new) }
  end

  it "should raise a TypeError if to_i doesn't return an Integer" do
    (obj = Object.new).should_receive(:to_i, :returning => "ha!")
    should_raise(TypeError) { Integer(obj) }
  end
end
