require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#remove_instance_variable" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("remove_instance_variable")
  end
end
