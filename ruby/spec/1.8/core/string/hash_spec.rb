require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#hash" do
  it "returns a hash based on a string's length and content" do
    "abc".hash.should == "abc".hash
    "abc".hash.should_not == "cba".hash
  end
end
