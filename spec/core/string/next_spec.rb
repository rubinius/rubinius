require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#next" do
  it "is an alias of String#succ" do
    "".succ.should == ""
    "\xFF".succ.should == "\x01\x00"
    "abcd".succ.should == "abcd".next
    "98".succ.should == "98".next
    "ZZZ9999".succ.should == "ZZZ9999".next
  end
  
  it "taints the result if self is tainted" do
    "".taint.succ.tainted?.should == true
    "x".taint.succ.tainted?.should == true
  end
end

describe "String#next!" do
  it "is an alias of String#succ!" do
    a = "THX1138"
    b = "THX1138"
    a.succ!.should == b.next!
    a.should == b
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "".freeze.succ! }
    should_raise(TypeError) { "a".freeze.succ! }
  end
end
