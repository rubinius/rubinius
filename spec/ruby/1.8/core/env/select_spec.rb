require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV#select" do

  it "returns the Hash for which block return true" do
    ENV["foo"] = "bar"
    ENV.select { |k, v| k == "foo" }.should == [["foo", "bar"]]
    ENV.delete "foo"
  end

  it "raises when no block is given" do
    lambda { ENV.select }.should raise_error(LocalJumpError)
  end
end
