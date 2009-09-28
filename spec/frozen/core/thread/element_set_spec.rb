require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#[]=" do
  ruby_version_is ""..."1.9" do
    it "raises exceptions on the wrong type of keys" do
      lambda { Thread.current[nil] = true }.should raise_error(TypeError)
      lambda { Thread.current[5] = true }.should raise_error(ArgumentError)
    end
  end

  ruby_version_is "1.9" do
    it "raises exceptions on the wrong type of keys" do
      lambda { Thread.current[nil] = true }.should raise_error(TypeError)
      lambda { Thread.current[5] = true }.should raise_error(TypeError)
    end
  end
end
