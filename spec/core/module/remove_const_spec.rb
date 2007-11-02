require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#remove_const" do
  it "returns the value of the removed constant" do
    ModuleSpecs::Internal::ONE = 1
    ModuleSpecs::Internal.send(:remove_const, :ONE).should == 1
    should_raise(NameError) do
      ModuleSpecs::Internal.send(:remove_const, :ONE)
    end
  end

  it "is a private method" do 
    ModuleSpecs::Internal::TWO = 2
    should_raise(NameError) do
      ModuleSpecs::Internal.remove_const(:TWO)
    end
    ModuleSpecs::Internal.send(:remove_const, :TWO).should == 2
  end
end

