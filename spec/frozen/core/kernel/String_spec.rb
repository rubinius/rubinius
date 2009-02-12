require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.String" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:String)
  end
  
  it "converts the given argument to a String by calling #to_s" do
    Kernel.String(nil).should == ""
    Kernel.String("10").should == "10"
    Kernel.String(1.12).should == "1.12"
    Kernel.String(false).should == "false"
    Kernel.String(Object).should == "Object"

    (obj = mock('test')).should_receive(:to_s).and_return("test")
    Kernel.String(obj).should == "test"
  end

# TODO: does not work yet because of undef_method
#
#  it "raises a TypeError of #to_s is not provided" do
#    class << (obj = mock('to_s'))
#      undef_method :to_s
#    end
#    
#    lambda {
#      Kernel.String(obj)
#    }.should raise_error(TypeError)
#  end
  
  it "raises a TypeError if #to_s does not return a String" do
    (obj = mock('123')).should_receive(:to_s).and_return(123)
    lambda { Kernel.String(obj) }.should raise_error(TypeError)
  end
end

describe "Kernel#String" do
  it "needs to be reviewed for spec completeness"
end
