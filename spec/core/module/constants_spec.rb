require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module.constants" do
  it "returns an array of the names of all constants defined" do
    a = Module.constants.size
    ABC = ""
    b = Module.constants.size.should == a + 1
  end
end

describe "Module#constants" do
  it "returns an array with the names of all constants accessible in the scope of self" do
    ModuleSpecs.constants.sort.should == ["BasicModule", "Child", "Parent", "SuperModule"]
    
    Module.new { const_set :A, "test" }.constants.should == [ "A" ]
    Class.new  { const_set :A, "test" }.constants.should == [ "A" ]
  end
end
