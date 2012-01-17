require File.expand_path('../../../../spec_helper', __FILE__)

describe "FFI::MemoryPointer#write_pointer" do
  it "writes another pointer to self" do
    pointer = FFI::MemoryPointer.new :pointer
    pointer.write_pointer FFI::Pointer.new(0x0)
    pointer.read_pointer.address.should == 0x0
  end
end
