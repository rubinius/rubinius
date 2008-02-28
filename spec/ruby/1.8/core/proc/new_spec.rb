require File.dirname(__FILE__) + '/fixtures/procs.rb'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc.new" do
  it "returns a new Proc instance from the passed block" do
    Proc.new { }.call.should == nil
    Proc.new { "hello" }.call.should == "hello"
  end
  
  it "raises an ArgumentError when no block was passed" do
    raise_error(ArgumentError, "tried to create Proc object without a block") do
      Proc.new
    end 
  end

  it "raises a LocalJumpError when context of block no longer exists" do
    def some_method(&b) b end
    a_proc = Proc.new { return } 
    res = some_method(&a_proc)

    raise_error(LocalJumpError) { res.call }
  end

  it "returns from within a method" do
    ProcSpecs.returner
  end

  it "raises an ArgumentError when called without a block inside a method without an attached block" do
    def some_method() Proc.new end
    lambda { some_method }.should raise_error(ArgumentError)
  end

  it "returns a new Proc instance from the block passed to the containing method" do
    def some_method() Proc.new end
    some_method { "hello" }.call.should == "hello" 
  end
end
