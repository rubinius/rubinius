require File.expand_path('../../../spec_helper', __FILE__)

describe :singleton_method_undefined, :shared => true do
  it "is a private method" do
    @object.should have_private_instance_method(@method)
  end
end
