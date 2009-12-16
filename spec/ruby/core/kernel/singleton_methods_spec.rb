require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# Prior to MRI 1.9 #singleton_methods returned an Array of Strings
ruby_version_is ""..."1.9" do
  describe "Kernel#singleton_methods" do
    it "returns a list of the names of singleton methods in the object" do
      m = KernelSpecs::Methods.singleton_methods(false)
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"].each do |e|
        m.should include(e)
      end
      
      KernelSpecs::Methods.new.singleton_methods(false).should == []
    end

    it "returns a list of names of singleton methods in object and superclasses when passed true" do |e|
      x = Class.new {
        def self.cls_inherited; end
      }
      y = Class.new(x)

      m0 = x.singleton_methods(false)
      m1 = y.singleton_methods(false)
      m2 = x.singleton_methods(true)
      m3 = y.singleton_methods(true)

      m0.should include "cls_inherited"
      m1.should_not include "cls_inherited"
      m2.should include "cls_inherited"
      m3.should include "cls_inherited"
    end
    
    it "should handle singleton_methods call with and without argument" do
      [1, "string", :symbol, [], {} ].each do |e|
        lambda {e.singleton_methods}.should_not raise_error
        lambda {e.singleton_methods(true)}.should_not raise_error
        lambda {e.singleton_methods(false)}.should_not raise_error
      end
      
    end
    
    it "returns a list of the names of singleton methods in the object and its ancestors and mixed-in modules" do
      m = (KernelSpecs::Methods.singleton_methods(false) & KernelSpecs::Methods.singleton_methods)
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"].each do |e|
        m.should include(e)
      end
      
      KernelSpecs::Methods.new.singleton_methods.should == []
    end
  end
end

# In MRI 1.9 #singleton_methods returns an Array of Symbols
ruby_version_is "1.9" do
  describe "Kernel#singleton_methods" do
    it "returns a list of the names of singleton methods in the object" do
      m = KernelSpecs::Methods.singleton_methods(false)
      [:hachi, :ichi, :juu, :juu_ichi, :juu_ni, :roku, :san, :shi].each do |e|
        m.should include(e)
      end
      
      KernelSpecs::Methods.new.singleton_methods(false).should == []
    end
    
    it "returns a list of names of singleton methods in object and superclasses when passed true" do |e|
      x = Class.new {
        def self.cls_inherited; end
      }
      y = Class.new(x)

      m0 = x.singleton_methods(false)
      m1 = y.singleton_methods(false)
      m2 = x.singleton_methods(true)
      m3 = y.singleton_methods(true)

      m0.should include :cls_inherited
      m1.should_not include :cls_inherited
      m2.should include :cls_inherited
      m3.should include :cls_inherited
    end
    
    it "should handle singleton_methods call with and without argument" do
      [1, "string", :symbol, [], {} ].each do |e|
        lambda {e.singleton_methods}.should_not raise_error
        lambda {e.singleton_methods(true)}.should_not raise_error
        lambda {e.singleton_methods(false)}.should_not raise_error
      end
      
    end
    
    it "returns a list of the names of singleton methods in the object and its ancestors and mixed-in modules" do
      m = (KernelSpecs::Methods.singleton_methods(false) & KernelSpecs::Methods.singleton_methods)
      [:hachi, :ichi, :juu, :juu_ichi, :juu_ni, :roku, :san, :shi].each do |e|
        m.should include(e)
      end
      
      KernelSpecs::Methods.new.singleton_methods.should == []
    end
  end
end  
