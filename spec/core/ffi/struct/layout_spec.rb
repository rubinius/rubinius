require File.expand_path('../../../../spec_helper', __FILE__)

describe "FFI::Struct.layout" do
  context "when class name is not set" do
    it "should not raise a error" do
      lambda {
        klass = Class.new(FFI::Struct)
        klass.layout :any, :int
      }.should_not raise_error
    end
  end
  
  
  it "needs to be reviewed for spec completeness"
end
