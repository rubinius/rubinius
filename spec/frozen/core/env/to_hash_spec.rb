require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.to_hash" do

  it "returns the ENV as a hash" do
    ENV["foo"] = "bar"
    h = ENV.to_hash
    h.is_a?(Hash).should == true
    h["foo"].should == "bar"
    ENV.delete "foo"
  end
end
