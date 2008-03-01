require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#initialize" do
  it "is called on subclasses" do
    m = ModuleSpecs::SubModule.new
    m.special.should == 10
    m.method_table.should_not == nil
    m.constants.should_not == nil
  end
end
