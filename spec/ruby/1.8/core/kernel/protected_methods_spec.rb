require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#protected_methods" do
  it "returns a list of the names of protected methods accessible in the object" do
    KernelSpecs::Methods.protected_methods(false).sort.should include("juu_ichi")
    KernelSpecs::Methods.new.protected_methods(false).should include("ku")
  end
  
  it "returns a list of the names of protected methods accessible in the object and from its ancestors and mixed-in modules" do
    l1 = KernelSpecs::Methods.protected_methods(false)
    l2 = KernelSpecs::Methods.protected_methods
    (l1 & l2).should include("juu_ichi")
    KernelSpecs::Methods.new.protected_methods.should include('ku')
  end
end
