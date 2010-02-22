require File.expand_path('../../../spec_helper', __FILE__)

describe "ENV.delete" do
  it "removes the variable and return its value" do
    ENV["foo"] = "bar"
    ENV.delete("foo").should == "bar"
    ENV["foo"].should == nil
  end
end
