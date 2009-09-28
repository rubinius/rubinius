require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#private_methods" do
  ruby_version_is ""..."1.9" do
    it "returns a list of the names of privately accessible methods in the object" do
      m = KernelSpecs::Methods.private_methods(false)
      m.should include("shichi")
      m = KernelSpecs::Methods.new.private_methods(false)
      m.should include("juu_shi")
    end
    
    it "returns a list of the names of privately accessible methods in the object and its ancestors and mixed-in modules" do
      m = (KernelSpecs::Methods.private_methods(false) & KernelSpecs::Methods.private_methods)
      
      m.should include("shichi")
      m = KernelSpecs::Methods.new.private_methods
      m.should include('juu_shi')
    end
  end

  ruby_version_is "1.9" do
    it "returns a list of the names of privately accessible methods in the object" do
      m = KernelSpecs::Methods.private_methods(false)
      m.should include(:shichi)
      m = KernelSpecs::Methods.new.private_methods(false)
      m.should include(:juu_shi)
    end
    
    it "returns a list of the names of privately accessible methods in the object and its ancestors and mixed-in modules" do
      m = (KernelSpecs::Methods.private_methods(false) & KernelSpecs::Methods.private_methods)
      
      m.should include(:shichi)
      m = KernelSpecs::Methods.new.private_methods
      m.should include(:juu_shi)
    end
  end
end
