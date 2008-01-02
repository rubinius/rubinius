require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#[]" do
  it "should raise exceptions on the wrong type of keys" do
    lambda { Thread.current[nil] = true }.should raise_error(TypeError)
    lambda { Thread.current[5] = true }.should raise_error(ArgumentError)
  end
end
