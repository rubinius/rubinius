require File.dirname(__FILE__) + '/../spec_helper'

describe ENV do
  it "should return the variable from []" do
    ENV["USER"].should == `whoami`.strip
  end

  it "should return nil from [] if the variable isn't found" do
    ENV["this_var_is_never_set"].should == nil
  end

  it "should set the variable to the given value in []=" do
    ENV["foo"] = "bar"
    `env | grep -e ^foo=`.strip.should == "foo=bar"
  end

  it "delete should remove the variable and return its value" do
    ENV["foo"] = "bar"
    ENV.delete("foo").should == "bar"
    ENV["foo"].should == nil
  end

  it "should return \"ENV\" from to_s" do
    ENV.to_s.should == "ENV"
  end
end

