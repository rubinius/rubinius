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
end
