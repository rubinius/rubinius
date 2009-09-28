require File.dirname(__FILE__) + '/../../spec_helper'

describe :singleton_method_added, :shared => true do
  it "is a private method" do
    @object.should have_private_instance_method(@method)
  end
end
