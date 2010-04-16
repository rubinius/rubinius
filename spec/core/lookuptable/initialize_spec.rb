require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
