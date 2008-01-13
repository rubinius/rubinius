require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#initialize_copy" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("initialize_copy")
  end
end
