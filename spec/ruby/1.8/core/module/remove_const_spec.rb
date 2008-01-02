require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#remove_const" do
  it "returns the value of the removed constant" do
    ModuleSpecs::Internal::ONE = 1
    ModuleSpecs::Internal.send(:remove_const, :ONE).should == 1
    lambda { ModuleSpecs::Internal.send(:remove_const, :ONE) }.should raise_error(NameError)
  end

  it "is a private method" do 
    ModuleSpecs::Internal::TWO = 2
    lambda { ModuleSpecs::Internal.remove_const(:TWO) }.should raise_error(NameError)
    ModuleSpecs::Internal.send(:remove_const, :TWO).should == 2
  end
end

