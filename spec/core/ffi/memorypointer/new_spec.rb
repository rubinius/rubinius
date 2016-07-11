require File.expand_path('../../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "FFI::MemoryPointer.new" do
  it "needs to be reviewed for spec completeness"
  
  it "returns null when the byte size is negative" do
    FFI::MemoryPointer.new(-1).should == FFI::Pointer::NULL
  end
  
  it "returns null when the count is negative" do
    FFI::MemoryPointer.new(:int, -3).should == FFI::Pointer::NULL
  end
end
