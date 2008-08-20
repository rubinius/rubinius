require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module.__add_method__" do
  it "adds a CompiledMethod to a class or module" do
    ModuleSpecs::A.a.should == :added
    ModuleSpecs::F.new.b.should == :added
  end

  it "sets the staticscope of the CompiledMethod to the enclosing class or module" do
    ModuleSpecs::A.method_table[:b][1].staticscope.module.should == ModuleSpecs::A
  end
end
