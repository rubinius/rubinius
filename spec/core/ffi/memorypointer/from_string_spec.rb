require File.expand_path('../../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "FFI::MemoryPointer.from_string" do
  it "creates a new FFI::MemoryPointer from a String" do
    ptr = FFI::MemoryPointer.from_string "abcd"
    ptr.total.should == 5
    ptr.read_string.should == "abcd"
  end
end
