require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@kernel_kind_of = shared "Kernel#kind_of" do |cmd|
  describe "Enumerable##{cmd}" do     
    # #kind_of? and #is_a are aliases, so we only need one function
    # that tests both methods  
    
    it "returns true if class is the class of obj, or if class is one of the superclasses of obj or modules included in obj (String example)" do    
      s = "hello"
      s.send(cmd, String).should == true
      s.send(cmd, Object).should == true
      s.send(cmd, Class).should == false 
    end

    it "returns true if class is the class of obj, or if class is one of the superclasses of obj or modules included in obj Array example" do
      a = []
      a.send(cmd, Array).should == true
      a.send(cmd, Enumerable).should == true
    end    
          
    module KernelSpecs::M;    end
    class KernelSpecs::X; include KernelSpecs::M; end
    class KernelSpecs::Y < KernelSpecs::X; end
    class KernelSpecs::Z < KernelSpecs::Y; end 
  
    it "returns true if class is the class of obj, or if class is one of the superclasses of obj or modules included in obj Custom class example" do     
      y = KernelSpecs::Y.new
      y.send(cmd, KernelSpecs::X).should == true       
      y.send(cmd, KernelSpecs::Y).should == true       
      y.send(cmd, KernelSpecs::Z).should == false       
      y.send(cmd, KernelSpecs::M).should == true
    end
    
    it "nil.#{cmd} cases specs"  do 
      (nil.send(cmd,  NilClass)).should == true
      (nil.send(cmd, Object)).should == true
      (nil.send(cmd, Module)).should == false
      (nil.send(cmd, Kernel)).should == true
      
    end
  end 
end

describe "Kernel#kind_of?" do
  it_behaves_like(@kernel_kind_of , :kind_of?) 
end
