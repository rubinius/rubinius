require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module.nesting" do
  it "returns the list of Modules nested at the point of call" do
    module ModuleSpecs
      Module.nesting.should == [ModuleSpecs]

      module BasicModule
        Module.nesting.should == [ModuleSpecs::BasicModule, ModuleSpecs]
      end

      class Parent
        Module.nesting.should == [ModuleSpecs::Parent, ModuleSpecs]
      end
    end
  end
end
