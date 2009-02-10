require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#rstrip" do
  it "returns a copy of self with trailing whitespace removed" do
   "  hello  ".rstrip.should == "  hello"
   "  hello world  ".rstrip.should == "  hello world"
   "  hello world \n\r\t\n\v\r".rstrip.should == "  hello world"
   "hello".rstrip.should == "hello"
   "hello\x00".rstrip.should == "hello"
  end

  it "taints the result when self is tainted" do
    "".taint.rstrip.tainted?.should == true
    "ok".taint.rstrip.tainted?.should == true
    "ok    ".taint.rstrip.tainted?.should == true
  end
end

describe "String#rstrip!" do
  it "modifies self in place and returns self" do
    a = "  hello  "
    a.rstrip!.should equal(a)
    a.should == "  hello"
  end

  it "returns nil if no modifications were made" do
    a = "hello"
    a.rstrip!.should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    "hello".freeze.rstrip! # ok, nothing changed
    "".freeze.rstrip! # ok, nothing changed

    lambda { "  hello  ".freeze.rstrip! }.should raise_error(TypeError)
  end
end
