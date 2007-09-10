require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Kernel#public_methods" do
  it "returns a list of the names of publicly accessible methods in the object" do
    KernelMethods.public_methods(false).sort.should == 
      ["allocate", "hachi", "ichi", "juu", "juu_ni", "new", "roku", "san", "shi", "superclass"]
    KernelMethods.new.public_methods(false).sort.should == ["juu_san", "ni"]
  end
  
  it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
    (KernelMethods.public_methods(false) & KernelMethods.public_methods).sort.should == 
      ["allocate", "hachi", "ichi", "juu", "juu_ni", "new", "roku", "san", "shi", "superclass"]
    m = KernelMethods.new.public_methods
    m.should_include('ni')
    m.should_include('juu_san')
  end
end
