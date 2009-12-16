require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#lstrip" do
  it "returns a copy of self with leading whitespace removed" do
   "  hello  ".lstrip.should == "hello  "
   "  hello world  ".lstrip.should == "hello world  "
   "\n\r\t\n\v\r hello world  ".lstrip.should == "hello world  "
   "hello".lstrip.should == "hello"
  end

  # spec/core/string/lstrip_spec.rb
  not_compliant_on :rubinius do
    it "does not strip leading \0" do
     "\x00hello".lstrip.should == "\x00hello"
    end
  end

  it "taints the result when self is tainted" do
    "".taint.lstrip.tainted?.should == true
    "ok".taint.lstrip.tainted?.should == true
    "   ok".taint.lstrip.tainted?.should == true
  end
end

describe "String#lstrip!" do
  it "modifies self in place and returns self" do
    a = "  hello  "
    a.lstrip!.should equal(a)
    a.should == "hello  "
  end

  it "returns nil if no modifications were made" do
    a = "hello"
    a.lstrip!.should == nil
    a.should == "hello"
  end

  ruby_version_is ""..."1.9" do 
    it "raises a TypeError if self is frozen" do
      "hello".freeze.lstrip! # ok, nothing changed
      "".freeze.lstrip! # ok, nothing changed

      lambda { "  hello  ".freeze.lstrip! }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do 
    ruby_bug "#1550", "1.9.2" do
      it "raises a RuntimeError if self is frozen" do
        lambda { "hello".freeze.lstrip! }.should raise_error(RuntimeError)
        lambda { "".freeze.lstrip!      }.should raise_error(RuntimeError)
        lambda { "  hello  ".freeze.lstrip! }.should raise_error(RuntimeError)
      end
    end
  end
end
