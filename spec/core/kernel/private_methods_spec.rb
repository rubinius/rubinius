require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Kernel#private_methods" do
  it "returns a list of the names of privately accessible methods in the object" do
    KernelMethods.private_methods(false).sort.should == 
      ["inherited", "initialize", "initialize_copy", "shichi"]
    KernelMethods.new.private_methods(false).sort.should == ["juu_shi"]
  end
  
  it "returns a list of the names of privately accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.private_methods(false) & KernelMethods.private_methods).sort.should == 
      ["inherited", "initialize", "initialize_copy", "shichi"]
    m = KernelMethods.new.private_methods
    m.should_include('juu_shi')
  end
end
