require File.expand_path('../../spec_helper', __FILE__)

# TODO: Obsolete. Rework if appropriate for FFI::Library#attach_function

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

  it "doesn't count the first :state argument in arity" do
    func = FFI.create_function(FFI::USE_THIS_PROCESS_AS_LIBRARY,
                               'abs', [:state], :int)
    # Note :state is a wrong parameter for the abs function.
    # However, we're only testing the arity here, and using
    # the state (will be a pointer) as the argument to abs
    # will not cause a problem (though the result will be
    # "garbage").
    lambda { func.call() }.should_not raise_error(ArgumentError)
  end
end
