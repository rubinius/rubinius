require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#__add_method__" do
  it "adds a CompiledMethod to a class or module" do
    KernelSpecs::D.new.a
    KernelSpecs::D.new.c.should == :added
  end

  it "sets the staticscope of the CompiledMethod to the enclosing class or module" do
    KernelSpecs::D.new.a
    KernelSpecs::D.method_table[:c][1].staticscope.module.should == KernelSpecs::D
  end
end
