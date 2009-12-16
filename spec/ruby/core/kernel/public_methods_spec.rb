require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#public_methods" do
  ruby_version_is ""..."1.9" do
    it "returns a list of the names of publicly accessible methods in the object" do
      KernelSpecs::Methods.public_methods(false).sort.should include("allocate", "hachi",
        "ichi", "juu", "juu_ni", "new", "roku", "san", "shi", "superclass")
      KernelSpecs::Methods.new.public_methods(false).sort.should include("juu_san", "ni")
    end
    
    it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
      (KernelSpecs::Methods.public_methods(false) & KernelSpecs::Methods.public_methods).sort.should include(
        "allocate", "hachi", "ichi", "juu", "juu_ni", "new", "roku", "san", "shi", "superclass")
      m = KernelSpecs::Methods.new.public_methods
      m.should include('ni', 'juu_san')
    end
  end

  ruby_version_is "1.9" do
    it "returns a list of the names of publicly accessible methods in the object" do
      KernelSpecs::Methods.public_methods(false).sort.should include(:allocate, :hachi,
        :ichi, :juu, :juu_ni, :new, :roku, :san, :shi, :superclass)
      KernelSpecs::Methods.new.public_methods(false).sort.should include(:juu_san, :ni)
    end
    
    it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
      (KernelSpecs::Methods.public_methods(false) & KernelSpecs::Methods.public_methods).sort.should include(
        :allocate, :hachi, :ichi, :juu, :juu_ni, :new, :roku, :san, :shi, :superclass)
      m = KernelSpecs::Methods.new.public_methods
      m.should include(:ni, :juu_san)
    end
  end
end
