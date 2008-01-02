require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.[]=" do
  it "set the environment variable to the given value" do
    ENV["foo"] = "bar"
    `env | grep -e ^foo=`.strip.should == "foo=bar"
  end
end
