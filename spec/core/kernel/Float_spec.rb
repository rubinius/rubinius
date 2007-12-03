require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.Float when passed a String" do
  it "raises a TypeError when the given String can't be fully converted to a Float" do
    lambda { Float('0.0.0')  }.should raise_error(ArgumentError)
    lambda { Float('float')  }.should raise_error(ArgumentError)
    lambda { Float('10.0:D') }.should raise_error(ArgumentError)
    lambda { Float('D10')    }.should raise_error(ArgumentError)
  end
end

describe "Kernel.Float" do
  it "converts the given argument to a Float by calling #to_f" do
    Kernel.Float(1).should == 1.0
    Kernel.Float(1.12).should == 1.12
    Kernel.Float(1000000000000).should == 1000000000000.0
    Kernel.Float("10").should == 10.0
    
    (obj = Object.new).should_receive(:to_f, :returning => 1.2)
    obj.should_not_receive(:to_i)
    Kernel.Float(obj).should == 1.2
  end
  
  it "raises a TypeError of #to_f is not provided" do
    lambda { Kernel.Float(Object.new) }.should raise_error(TypeError)
  end
  
  it "raises a TypeError if #to_f does not return a Float" do
    (obj = Object.new).should_receive(:to_f, :returning => 'ha!')
    lambda { Kernel.Float(obj) }.should raise_error(TypeError)

    obj.should_receive(:to_f, :returning => 123)
    lambda { Kernel.Float(obj) }.should raise_error(TypeError)
  end
end
