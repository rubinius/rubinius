require File.dirname(__FILE__) + '/spec_helper'

load_extension("register_address")

describe "CApiRegisterAddressSpecs" do
  before :each do
    @f = CApiRegisterAddressSpecs.new
  end
  
  it "correctly gets the value from a registered address" do
    @f.registered_tagged_address.should == 10
    @f.registered_tagged_address.object_id.should == @f.registered_tagged_address.object_id
    @f.registered_reference_address.should == "Globally registered data"
    @f.registered_reference_address.object_id.should == @f.registered_reference_address.object_id
  end
end
