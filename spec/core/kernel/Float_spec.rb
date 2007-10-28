require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.Float when passed a String" do
  it "raises a TypeError when the given String can't be fully converted to a Float" do
    should_raise(ArgumentError, 'invalid value for Float(): "0.0.0"') do
      Float('0.0.0')
    end
    
    should_raise(ArgumentError, 'invalid value for Float(): "float"') do
      Float('float')
    end
    
    should_raise(ArgumentError, 'invalid value for Float(): "10.0:D"') do
      Float('10.0:D')
    end
    
    should_raise(ArgumentError, 'invalid value for Float(): "D10"') do
      Float('D10')
    end
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
    should_raise(TypeError) do
      Kernel.Float(Object.new)
    end
  end
  
  it "raises a TypeError if #to_f does not return a Float" do
    (obj = Object.new).should_receive(:to_f, :returning => 'ha!')
    should_raise(TypeError) do
      Kernel.Float(obj)
    end

    obj.should_receive(:to_f, :returning => 123)
    should_raise(TypeError) do
      Kernel.Float(obj)
    end
  end
end
