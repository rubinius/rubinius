require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/method'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#method" do
  it_behaves_like(:kernel_method, :method)

  before(:each) do
    @obj = KernelSpecs::A.new
  end

  # The following examples are to clarify the difference between #method and
  # #public_method on 1.9
  it "can be called on a private method" do
    @obj.send(:private_method).should == :private_method
    @obj.method(:private_method).should be_an_instance_of(Method)
  end

  it "raises a NameError when called on a protected method" do
    @obj.send(:protected_method).should == :protected_method
    @obj.method(:protected_method).should be_an_instance_of(Method)
  end
end
