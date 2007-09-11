require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#method_missing" do
  class KernelSpecs::A
    def method_missing (*args)
      "a_new_method_missing"
    end
  end 
   
  it  "return the correct value form method_missing after call a not defined instance method" do
    KernelSpecs::A.new.foo.should == "a_new_method_missing"
  end
  
  it "raise a NoMethodError Exceptiong after call a not defined Class method even if te the method_missing its defined" do  
    should_raise(NoMethodError){KernelSpecs::A.foo.should == "a_new_method_missing"} 
  end    
   
  class KernelSpecs::B;end   
  
  it "raise a NoMethodError Exceptiong after call a not defined instance method" do
    should_raise(NoMethodError){KernelSpecs::B.new.foo}
  end

  
  it  "raise a NoMethodError exception after call a non defined method " do
    should_raise(NoMethodError){some_method "a", 1} 
  end    
  
  class KernelSpecs::C
    def self.method_missing (*args)
      "a_new_method_missing"
    end
  end
    
  it  "return the correct value form method_missing after call a not defined class method" do
    KernelSpecs::C.foo.should == "a_new_method_missing"
  end
  
  it  "raise a NoMethodError Exceptiong after call a not defined instance method" do 
    should_raise(NoMethodError){KernelSpecs::C.new.foo.should == "a_new_method_missing"} 
  end
   

  class KernelSpecs::AParent
    def method_missing (*args)
      "a_new_method_missing"
    end
  end
    
  class KernelSpecs::AChild < KernelSpecs::AParent
    def foo
      super
    end
  end    
    
  it  "return the correct value form method_missing after call a not defined instance method" do
    KernelSpecs::AChild.new.foo.should == "a_new_method_missing"
  end
   
  class KernelSpecs::ProtectedAndPrivate
    private
    def a_private_method;  end 
    protected
    def a_protected_method; end
  end
    
  class KernelSpecs::ProtectedAndMissing 
    def method_missing(*args)
      :a_new_method_missing
    end
    protected 
    def self.a_protected_class_method
      :a_protected_class_method
    end
  end
    
  class KernelSpecs::PrivateAndMissing
    def method_missing(*args)
      :a_new_method_missing
    end    
    private 
    def self.a_private_class_method
      :a_private_class_method
    end 
  end 
   
  it  "raise a NoMethodError exception after call a non defined method in a class with proctected and private methods" do
    should_raise(NoMethodError){KernelSpecs::ProtectedAndPrivate.new.a_missing_method}
  end  
  
  it  "raise a not a NoMethodError and message should be a exception after call a private methor defined in a class " do
    should_raise(NoMethodError){KernelSpecs::ProtectedAndPrivate.new.a_private_method} 
  end
  
  it  "raise a not a NoMethodError and message should be a exception after call a protected methor defined in a class " do
    should_raise(NoMethodError){KernelSpecs::ProtectedAndPrivate.new.a_protected_method}
  end
  
  it  "return the correct value from the method_missing method after call a private instance method" do
    KernelSpecs::PrivateAndMissing.new.a_private_method.should == :a_new_method_missing
  end
  
  it  "return the correct value from the method_missing method after call a protected instance method" do
    KernelSpecs::ProtectedAndMissing.new.a_protected_method.should == :a_new_method_missing
  end
  
  it  "return the correct value from the method_missing method after call a private class method" do
   KernelSpecs::PrivateAndMissing.a_private_class_method.should == :a_private_class_method
  end
    
  it  "return the correct value from the method_missing method after call a protected class method" do
    KernelSpecs::ProtectedAndMissing.a_protected_class_method.should == :a_protected_class_method
  end 
end
