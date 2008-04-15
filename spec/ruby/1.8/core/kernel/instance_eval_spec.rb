require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#instance_eval" do
  it "expects a block with no arguments" do
    lambda { "hola".instance_eval }.should raise_error(ArgumentError)
  end
  
  it "takes no arguments with a block" do
    lambda { "hola".instance_eval(4, 5) { |a,b| a + b } }.should raise_error(ArgumentError)
  end
  
  it "passes the object to the block" do
    "hola".instance_eval { |o| o.size }.should == 4
  end
  
  it "binds self to the receiver" do
    s = "hola"
    (s == s.instance_eval { self }).should == true
    o = mock('o')
    (o == o.instance_eval("self")).should == true
  end
  
  it "executes in the context of the receiver" do
    "Ruby-fu".instance_eval { size }.should == 7
    "hola".instance_eval("size").should == 4
  end

  it "has access to receiver's instance variables" do
    KernelSpecs::IVars.new.instance_eval { @secret }.should == 99
    KernelSpecs::IVars.new.instance_eval("@secret").should == 99
  end
  
  it "sets class variables in the receiver" do
    KernelSpecs::IncludesInstEval.class_variables.should include("@@count")
    KernelSpecs::IncludesInstEval.send(:class_variable_get, :@@count).should == 2
  end

  it "raises a TypeError when defining methods on an immediate" do
    lambda do
      1.instance_eval { def foo; end }
    end.should raise_error(TypeError)
    lambda do
      :foo.instance_eval { def foo; end }
    end.should raise_error(TypeError)
  end

quarantine! do # Not clean, leaves cvars lying around to break other specs
  it "scopes class var accesses in the caller when called on a Fixnum" do
    # Fixnum can take instance vars
    Fixnum.class_eval "@@__tmp_instance_eval_spec = 1"
    (defined? @@__tmp_instance_eval_spec).should == nil

    @@__tmp_instance_eval_spec = 2
    1.instance_eval { @@__tmp_instance_eval_spec }.should == 2
    Fixnum.__send__(:remove_class_variable, :@@__tmp_instance_eval_spec)
  end
end

  it "raises a TypeError when defining methods on numerics" do
    lambda do
      (1.0).instance_eval { def foo; end }
    end.should raise_error(TypeError)
    lambda do
      (1 << 64).instance_eval { def foo; end }
    end.should raise_error(TypeError)
  end
end
