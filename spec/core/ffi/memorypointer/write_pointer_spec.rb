require File.expand_path('../../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "FFI::MemoryPointer#write_pointer" do
  it "writes another pointer to self" do
    pointer = FFI::MemoryPointer.new :pointer
    pointer.write_pointer FFI::Pointer.new(0xdeadbeef)
    pointer.read_pointer.address.should == 0xdeadbeef
  end
end
