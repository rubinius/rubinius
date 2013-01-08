require File.expand_path('../../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "FFI::MemoryPointer#write_array_of_pointer" do
  it "writes an array of pointers to self" do
    values = %w[a b]
    ptrs = values.map { |s| FFI::MemoryPointer.from_string s }

    pointer = FFI::MemoryPointer.new :pointer, ptrs.size
    pointer.write_array_of_pointer ptrs
    result = pointer.read_array_of_pointer ptrs.size

    result.should == ptrs
    result.map { |e| e.read_string }.should == values
  end
end
