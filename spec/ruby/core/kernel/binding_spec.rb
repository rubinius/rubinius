require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#binding" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:binding)
  end
  
  before(:each) do
    @b1 = KernelSpecs::Binding.new(99).get_binding
  end

  it "returns a Binding object" do
    @b1.kind_of?(Binding).should == true
  end

  it "encapsulates the execution context properly" do
    eval("@secret", @b1).should == 100
    eval("a", @b1).should == true
    eval("b", @b1).should == true
    eval("@@super_secret", @b1).should == "password"

    eval("square(2)", @b1).should == 4
    eval("self.square(2)", @b1).should == 4

    eval("a = false", @b1)
    eval("a", @b1).should == false
  end

  it "raises a NameError on undefined variable" do
    lambda { eval("a_fake_variable", @b1) }.should raise_error(NameError)
  end
end

describe "Kernel.binding" do
  it "needs to be reviewed for spec completeness"
end
