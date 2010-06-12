require File.expand_path('../../../../spec_helper', __FILE__)

module FFIStructSpecs
  DummyLayout = [:a, :int, 0]

  class DummyStruct < FFI::Struct
    layout *DummyLayout
  end
end

describe "FFI::Struct.new" do
  it "creates pre-configured struct with proper size" do
    dummy = FFIStructSpecs::DummyStruct.new
    dummy.size.should == FFI.type_size(FFI.find_type(:int))
  end

  it "creates struct configured on the fly with proper size" do
    dummy = FFI::Struct.new(nil, *FFIStructSpecs::DummyLayout)
    dummy.size.should == FFI.type_size(FFI.find_type(:int))
  end

  it "needs to be reviewed for spec completeness"
end
