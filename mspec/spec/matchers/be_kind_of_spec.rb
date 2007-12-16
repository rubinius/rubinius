require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../matchers/be_kind_of'

describe BeKindOfMatcher do
  it "matches when actual is a kind_of? expected" do
    BeKindOfMatcher.new(Integer).matches?(1).should == true
    BeKindOfMatcher.new(Fixnum).matches?(2).should == true
    BeKindOfMatcher.new(Regexp).matches?(/m/).should == true
  end
  
  it "does not match when actual is not a kind_of? expected" do
    BeKindOfMatcher.new(Integer).matches?(1.5).should == false
    BeKindOfMatcher.new(String).matches?(:a).should == false
    BeKindOfMatcher.new(Hash).matches?([]).should == false
  end
  
  it "provides a useful failure message" do
    matcher = BeKindOfMatcher.new(Numeric)
    matcher.matches?('string')
    matcher.failure_message.should == ["Expected \"string\"", "to be kind of Numeric"]
  end
  
  it "provides a useful negative failure message" do
    matcher = BeKindOfMatcher.new(Numeric)
    matcher.matches?(4.2)
    matcher.negative_failure_message.should == ["Expected 4.2", "not to be kind of Numeric"]
  end
end
