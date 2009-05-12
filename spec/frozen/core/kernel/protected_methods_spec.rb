require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# The reason why having include() and ruby_version_is is to show the specification explicitly.
# You should use have_protected_method() with the exception of this spec.
describe "Kernel#protected_methods" do
  ruby_version_is "" ... "1.9" do
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

  ruby_version_is "1.9" do
    it "returns a list of the names of protected methods accessible in the object" do
      KernelSpecs::Methods.protected_methods(false).sort.should include(:juu_ichi)
      KernelSpecs::Methods.new.protected_methods(false).should include(:ku)
    end

    it "returns a list of the names of protected methods accessible in the object and from its ancestors and mixed-in modules" do
      l1 = KernelSpecs::Methods.protected_methods(false)
      l2 = KernelSpecs::Methods.protected_methods
      (l1 & l2).should include(:juu_ichi)
      KernelSpecs::Methods.new.protected_methods.should include(:ku)
    end
  end
end
