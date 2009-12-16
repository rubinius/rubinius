require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#protected_instance_methods" do
  ruby_version_is ""..."1.9" do
    it "returns a list of protected methods in module and its ancestors" do
      methods = ModuleSpecs::CountsMixin.protected_instance_methods
      methods.should include('protected_3')
    
      methods = ModuleSpecs::CountsParent.protected_instance_methods
      methods.should include('protected_3')
      methods.should include('protected_2')

      methods = ModuleSpecs::CountsChild.protected_instance_methods
      methods.should include('protected_3')
      methods.should include('protected_2')
      methods.should include('protected_1')
    end

    it "when passed false as a parameter, should return only methods defined in that module" do
      ModuleSpecs::CountsMixin.protected_instance_methods(false).should == ['protected_3']
      ModuleSpecs::CountsParent.protected_instance_methods(false).should == ['protected_2']
      ModuleSpecs::CountsChild.protected_instance_methods(false).should == ['protected_1']
    end
  end

  ruby_version_is "1.9" do
    it "returns a list of protected methods in module and its ancestors" do
      methods = ModuleSpecs::CountsMixin.protected_instance_methods
      methods.should include(:protected_3)
    
      methods = ModuleSpecs::CountsParent.protected_instance_methods
      methods.should include(:protected_3)
      methods.should include(:protected_2)

      methods = ModuleSpecs::CountsChild.protected_instance_methods
      methods.should include(:protected_3)
      methods.should include(:protected_2)
      methods.should include(:protected_1)
    end

    it "when passed false as a parameter, should return only methods defined in that module" do
      ModuleSpecs::CountsMixin.protected_instance_methods(false).should == [:protected_3]
      ModuleSpecs::CountsParent.protected_instance_methods(false).should == [:protected_2]
      ModuleSpecs::CountsChild.protected_instance_methods(false).should == [:protected_1]
    end
  end

  it "default list should be the same as passing true as an argument" do
    ModuleSpecs::CountsMixin.protected_instance_methods(true).should ==
      ModuleSpecs::CountsMixin.protected_instance_methods
    ModuleSpecs::CountsParent.protected_instance_methods(true).should ==
      ModuleSpecs::CountsParent.protected_instance_methods
    ModuleSpecs::CountsChild.protected_instance_methods(true).should ==
      ModuleSpecs::CountsChild.protected_instance_methods
  end
end
