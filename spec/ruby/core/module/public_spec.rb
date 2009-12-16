require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#public" do
  it "on a superclass method calls the redefined method" do
    ModuleSpecs::ChildPrivateMethodMadePublic.new.private_method_redefined.should == :after_redefinition
  end
end
