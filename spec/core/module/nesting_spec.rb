require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module.nesting" do
  it "returns the list of Modules nested at the point of call" do
    module ModuleSpecs
      Module.nesting.should == [ModuleSpecs]

      module Basic
        Module.nesting.should == [ModuleSpecs::Basic, ModuleSpecs]
      end

      class Parent
        Module.nesting.should == [ModuleSpecs::Parent, ModuleSpecs]
      end
    end
  end
end
