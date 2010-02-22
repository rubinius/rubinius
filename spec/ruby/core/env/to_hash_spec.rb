require File.expand_path('../../../spec_helper', __FILE__)

describe "ENV.to_hash" do

  it "returns the ENV as a hash" do
    ENV["foo"] = "bar"
    h = ENV.to_hash
    h.is_a?(Hash).should == true
    h["foo"].should == "bar"
    ENV.delete "foo"
  end
end
