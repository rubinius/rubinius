require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module.constants" do
  it "returns an array of the names of all constants defined" do
    a = ModuleSpecs::AddConstant.constants.size
    ModuleSpecs::AddConstant::ABC = ""
    b = ModuleSpecs::AddConstant.constants.size.should == a + 1
  end
end

describe "Module#constants" do
  it "returns an array with the names of all accessible constants" do
    ModuleSpecs.constants.sort.should include("Basic", "Child", "CountsChild", 
      "CountsMixin", "CountsParent", "Parent", "Super")
    
    Module.new { const_set :A, "test" }.constants.should == [ "A" ]
    Class.new  { const_set :A, "test" }.constants.should == [ "A" ]
  end
end
