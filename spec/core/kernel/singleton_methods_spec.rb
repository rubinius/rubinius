require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#singleton_methods" do
  it "returns a list of the names of singleton methods in the object" do
    KernelSpecs::Methods.singleton_methods(false).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    KernelSpecs::Methods.new.singleton_methods(false).should == []
  end
  
  it "returns a list of the names of singleton methods in the object and its ancestors and mixed-in modules" do
    (KernelSpecs::Methods.singleton_methods(false) & KernelSpecs::Methods.singleton_methods).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    KernelSpecs::Methods.new.singleton_methods.should == []
  end
end
