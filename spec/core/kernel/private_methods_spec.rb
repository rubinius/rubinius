require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#private_methods" do
  it "returns a list of the names of privately accessible methods in the object" do
    KernelSpecs::Methods.private_methods(false).sort.should == 
      ["inherited", "initialize", "initialize_copy", "shichi"]
    KernelSpecs::Methods.new.private_methods(false).sort.should == ["juu_shi"]
  end
  
  it "returns a list of the names of privately accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelSpecs::Methods.private_methods(false) & KernelSpecs::Methods.private_methods).sort.should == 
      ["inherited", "initialize", "initialize_copy", "shichi"]
    m = KernelSpecs::Methods.new.private_methods
    m.should include('juu_shi')
  end
end
