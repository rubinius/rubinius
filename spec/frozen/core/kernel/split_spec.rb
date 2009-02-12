require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#split" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:split)
  end
end

describe "Kernel.split" do
  it "needs to be reviewed for spec completeness"
end
