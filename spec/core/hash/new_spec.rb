require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash.new" do
  it "creates a new Hash with default object if pass a default argument " do
    Hash.new(5).default.should == 5
    Hash.new({}).default.should == {}
  end

  it "does not create a copy of the default argument" do
    str = "foo"
    Hash.new(str).default.equal?(str).should == true
  end
  
  it "creates a Hash with a default_proc if passed a block" do
    Hash.new.default_proc.should == nil
    
    h = Hash.new { |x| "Answer to #{x}" }
    h.default_proc.call(5).should == "Answer to 5"
    h.default_proc.call("x").should == "Answer to x"
  end
  
  it "raises ArgumentError if more than one argument is passed" do
    should_raise(ArgumentError) { Hash.new(5,6) }
  end
  
  it "raises ArgumentError if passed both default argument and default block" do
    should_raise(ArgumentError) { Hash.new(5) { 0 } }
    should_raise(ArgumentError) { Hash.new(nil) { 0 } }
  end
end
