require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Kernel#protected_methods" do
  it "returns a list of the names of protected methods accessible in the object" do
    KernelMethods.protected_methods(false).sort.should == ["juu_ichi"]
    KernelMethods.new.protected_methods(false).should == ["ku"]
  end
  
  it "returns a list of the names of protected methods accessible in the object and from its ancestors and mixed-in modules" do
    (KernelMethods.protected_methods(false) & KernelMethods.protected_methods).sort.should == ["juu_ichi"]
    KernelMethods.new.protected_methods.should_include('ku')
  end
end
