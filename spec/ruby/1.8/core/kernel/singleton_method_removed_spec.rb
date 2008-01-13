require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#singleton_method_removed" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("singleton_method_removed")
  end
end
