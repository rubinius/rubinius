require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#singleton_method_undefined" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("singleton_method_undefined")
  end
end
