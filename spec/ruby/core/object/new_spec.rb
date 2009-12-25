require File.dirname(__FILE__) + '/../../spec_helper'

describe "Object.new" do
  it "creates a new Object" do
    Object.new.class.should == Object
  end

  ruby_version_is "1.9.2" do # Ref: [redmine:2451]
    it "accepts any number of arguments" do
      lambda {
        Object.new("This", "makes it easier", "to call super", "from other constructors")
      }.should_not raise_error
    end
  end
end

