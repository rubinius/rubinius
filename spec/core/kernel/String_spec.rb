require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.String" do
  it "raises TypeError if instance of BasicObject" do
    lambda { String(BasicObject.new) }.should raise_error(TypeError)
  end
end
