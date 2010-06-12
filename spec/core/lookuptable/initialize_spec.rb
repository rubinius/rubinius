require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::LookupTable#initialize" do
  it "is private" do
    Rubinius::LookupTable.private_instance_methods.should include("initialize")
  end

  it "is called on subclasses" do
    lt = LookupTableSpecs::SubTable.new(6)
    lt.size.should == 0
    lt.special.should == 6
  end
end
