require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#singleton_method_added" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:singleton_method_added)
  end
end
