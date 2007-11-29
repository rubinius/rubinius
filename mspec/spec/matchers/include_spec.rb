require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../matchers/include'

describe IncludeMatcher do
  it "matches when actual includes expected" do
    IncludeMatcher.new([1,2,3]).matches?(2).should == true
    IncludeMatcher.new("abc").matches?("b").should == true
    IncludeMatcher.new(1, 2, 3).matches?(3).should == true
  end
  
  it "does not match when actual does not include expected" do
    IncludeMatcher.new([1,2,3]).matches?(4).should == false
    IncludeMatcher.new("abc").matches?("d").should == false
    IncludeMatcher.new(1, 2, 3).matches?(4).should == false
  end
  
  it "matches when actual includes all expected" do
    IncludeMatcher.new([1,2,3]).matches?(3, 2, 1).should == true
    IncludeMatcher.new("abc").matches?("a", "b", "c").should == true
    IncludeMatcher.new(1, 2, 3).matches?(1, 2, 3).should == true
  end
  
  it "does not match when actual does not include all expected" do
    IncludeMatcher.new([1,2,3]).matches?(3, 2, 4).should == false
    IncludeMatcher.new("abc").matches?("a", "b", "c", "d").should == false
    IncludeMatcher.new(1, 2, 3).matches?(1, 5, 3).should == false
  end
  
  it "provides a useful failure message" do
    matcher = IncludeMatcher.new([1,2,3])
    matcher.matches?(5, 2)
    matcher.failure_message.should == ["Expected [1, 2, 3]", "to include [5, 2]"]
  end
  
  it "provides a useful negative failure message" do
    matcher = IncludeMatcher.new([1,2,3])
    matcher.matches?(1, 2, 3)
    matcher.negative_failure_message.should == ["Expected [1, 2, 3]", "not to include [1, 2, 3]"]
  end
end
