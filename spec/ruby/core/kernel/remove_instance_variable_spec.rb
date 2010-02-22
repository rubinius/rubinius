require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#remove_instance_variable" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:remove_instance_variable)
  end
end
