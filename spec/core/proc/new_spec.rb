require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc.new when called without a block inside a method with an attached block" do
  it "returns a new Proc instance from the block passed to the containing method" do
    def some_method() Proc.new end
    some_method { "hello" }.call.should == "hello" 
  end
end

describe "Proc.new when called without a block inside a method without an attached block" do
  it "raises an ArgumentError" do
    def some_method() Proc.new end
    should_raise(ArgumentError, "tried to create Proc object without a block") do
      some_method
    end 
  end
end

describe "Proc.new" do
  it "returns a new Proc instance from the passed block" do
    Proc.new { }.call.should == nil
    Proc.new { "hello" }.call.should == "hello"
  end
  
  it "raises an ArgumentError when no block was passed" do
    should_raise(ArgumentError, "tried to create Proc object without a block") do
      Proc.new
    end 
  end
end