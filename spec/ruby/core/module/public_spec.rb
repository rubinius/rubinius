require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Module#public" do
  it "on a superclass method calls the redefined method" do
    ModuleSpecs::ChildPrivateMethodMadePublic.new.private_method_redefined.should == :after_redefinition
  end
end
