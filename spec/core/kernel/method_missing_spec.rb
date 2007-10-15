require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#method_missing" do
  it "is not called when a defined method is called" do
    KernelSpecs::MethodMissing.should_not_receive(:method_missing)
    KernelSpecs::MethodMissingC.should_not_receive(:method_missing)
    obj = KernelSpecs::MethodMissingC.new
    obj.should_not_receive(:method_missing)
    
    KernelSpecs::MethodMissing.existing.should == :existing
    KernelSpecs::MethodMissingC.existing.should == :existing
    obj.existing.should == :instance_existing
  end
  
  it "is called when an undefined method is called" do
    KernelSpecs::MethodMissing.nonexistent.should == :method_missing
    KernelSpecs::MethodMissingC.nonexistent.should == :method_missing
    KernelSpecs::MethodMissingC.new.nonexistent.should == :instance_method_missing
  end
  
  it "is called when a private method is called" do
    KernelSpecs::MethodMissing.private_method.should == :method_missing
    KernelSpecs::MethodMissingC.private_method.should == :method_missing
    KernelSpecs::MethodMissingC.new.private_method.should == :instance_method_missing
  end
  
  it "is called when a protected method is called" do
    KernelSpecs::MethodMissingC.protected_method.should == :method_missing
    KernelSpecs::MethodMissingC.new.protected_method.should == :instance_method_missing
  end
end

describe "When Kernel#method_missing is undefined" do
  specify "a NoMethodError is raised when an undefined method is called" do
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissing.nonexistent }
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissingC.nonexistent }
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissingC.new.nonexistent }
  end
  
  specify "a NoMethodError is raised when a private method is called" do
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissing.nonexistent }
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissingC.nonexistent }
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissingC.new.nonexistent }
  end
  
  specify "a NoMethodError is raised when a protected method is called" do
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissing.nonexistent }
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissingC.nonexistent }
    should_raise(NoMethodError) { KernelSpecs::NoMethodMissingC.new.nonexistent }
  end
end
