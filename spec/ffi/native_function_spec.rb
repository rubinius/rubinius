require File.dirname(__FILE__) + '/../spec_helper'

describe "NativeFunction#call" do
  before :each do
    @func = FFI.create_function(FFI::USE_THIS_PROCESS_AS_LIBRARY,
                                'abs', [:int], :int)
  end
  
  it "calls OK when arguments matches" do
    @func.call(-5).should == 5
  end

  it "raises TypeError when argument type not match" do
    lambda { @func.call("foo") }.should raise_error(TypeError)
  end

  it "raises ArgumentError when argument too many" do
    lambda { @func.call(2, 3) }.should raise_error(ArgumentError)
  end

  it "raises ArgumentError when argument too few" do
    lambda { @func.call() }.should raise_error(ArgumentError)
  end
end
