require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#remove_instance_variable" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:remove_instance_variable)
  end
end
