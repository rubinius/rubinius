require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#protected_methods" do
  it "returns a list of the names of protected methods accessible in the object" do
    KernelSpecs::Methods.protected_methods(false).sort.should == ["juu_ichi"]
    KernelSpecs::Methods.new.protected_methods(false).should == ["ku"]
  end
  
  it "returns a list of the names of protected methods accessible in the object and from its ancestors and mixed-in modules" do
    (KernelSpecs::Methods.protected_methods(false) & KernelSpecs::Methods.protected_methods).sort.should == ["juu_ichi"]
    KernelSpecs::Methods.new.protected_methods.should_include('ku')
  end
end
