require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#reverse" do
  it "returns a new string with the characters of self in reverse order" do
    "stressed".reverse.should == "desserts"
    "m".reverse.should == "m"
    "".reverse.should == ""
  end

  it "taints the result if self is tainted" do
    "".taint.reverse.tainted?.should == true
    "m".taint.reverse.tainted?.should == true
  end
end

describe "String#reverse!" do
  it "reverses self in place and always returns self" do
    a = "stressed"
    a.reverse!.should equal(a)
    a.should == "desserts"

    "".reverse!.should == ""
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError if self is frozen" do
      "".freeze.reverse! # ok, no change
      lambda { "anna".freeze.reverse!  }.should raise_error(TypeError)
      lambda { "hello".freeze.reverse! }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    ruby_bug "[ruby-core:23666]", "1.9.2" do
      it "raises a RuntimeError if self is frozen" do
        lambda { "".freeze.reverse!      }.should raise_error(RuntimeError)
        lambda { "anna".freeze.reverse!  }.should raise_error(RuntimeError)
        lambda { "hello".freeze.reverse! }.should raise_error(RuntimeError)
      end
    end
  end  
end
