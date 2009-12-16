require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#abort" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:abort)
  end
end

describe "Kernel.abort" do
  it "needs to be reviewed for spec completeness"
end
