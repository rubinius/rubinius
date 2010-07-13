require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.respond_to?" do  
  it "indicates if a singleton object responds to a particular message" do
    class KernelSpecs::Foo; def self.bar; 'done'; end; end
    KernelSpecs::Foo.respond_to?(:bar).should == true
    KernelSpecs::Foo.respond_to?(:baz).should == false
  end
end

describe "Kernel#respond_to?" do
  before :each do 
    @a = KernelSpecs::A.new  
  end

  it "returns false if the given method was undefined" do
    @a.respond_to?(:undefed_method).should == false
    @a.respond_to?("undefed_method").should == false
  end

  it "returns true if obj responds to the given public method" do    
    @a.respond_to?(:pub_method).should == true
    @a.respond_to?("pub_method").should == true
  end
  
  it "returns true if obj responds to the given protected method (include_private = true)" do
    @a.respond_to?(:protected_method, true).should == true
    @a.respond_to?("protected_method", true).should == true
  end
  
  ruby_version_is ""..."1.9.3" do
    it "returns true if obj responds to the given protected method (include_private = false)" do
      @a.respond_to?(:protected_method, false).should == true
      @a.respond_to?("protected_method", false).should == true
    end
  end

  ruby_version_is "1.9.3" do
    it "returns false even if obj responds to the given protected method (include_private = false)" do
      @a.respond_to?(:protected_method, false).should == false
      @a.respond_to?("protected_method", false).should == false
    end
  end

  it "returns false even if obj responds to the given private method (include_private = false)" do
    @a.respond_to?(:private_method, false).should == false
    @a.respond_to?("private_method", false).should == false
  end
  
  it "returns true if obj responds to the given private method (include_private = true)" do
    @a.respond_to?(:private_method, true).should == true    
    @a.respond_to?("private_method", true).should == true
  end 

  it "indicates if an object responds to a particular message" do
    class KernelSpecs::Foo; def bar; 'done'; end; end
    KernelSpecs::Foo.new.respond_to?(:bar).should == true
    KernelSpecs::Foo.new.respond_to?(:invalid_and_silly_method_name).should == false
  end

  ruby_version_is "1.9" do
    platform_is :os => :windows do
      it "returns false for a method which exists but is unimplemented" do
        Process.methods.include?(:fork).should be_true
        Process.respond_to?(:fork).should be_false
        lambda { Process.fork }.should raise_error(NotImplementedError)  
      end
    end  

    platform_is :os => [:linux, :openbsd] do
      it "returns false for a method which exists but is unimplemented" do
        File.methods.include?(:lchmod).should be_true
        File.respond_to?(:lchmod).should be_false
        lambda { File.lchmod }.should raise_error(NotImplementedError)  
      end
    end  
  end
end
