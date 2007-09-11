require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#methods" do
  it "returns a list of the names of publicly accessible methods in the object" do
    KernelMethods.methods(false).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    KernelMethods.new.methods(false).should == []
  end
  
  it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.methods(false) & KernelMethods.methods).sort.should ==
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"]
    m = KernelMethods.new.methods
    m.should_include('ku')
    m.should_include('ni')
    m.should_include('juu_san')
  end
end
