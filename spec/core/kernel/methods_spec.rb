require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#methods" do
  it "returns a list of the names of publicly accessible methods in the object" do
    meths =  KernelSpecs::Methods.methods(false)

    ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"].each do|m|
      meths.should include(m)
    end
      
    KernelSpecs::Methods.new.methods(false).should == []
  end
  
  it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
    meths = KernelSpecs::Methods.methods(false) & KernelSpecs::Methods.methods
    ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"].each do |m|
      meths.should include(m)
    end
    
    m = KernelSpecs::Methods.new.methods
    m.should include('ku')
    m.should include('ni')
    m.should include('juu_san')
  end
end
