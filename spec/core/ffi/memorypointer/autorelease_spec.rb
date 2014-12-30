require File.expand_path('../../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "FFI::MemoryPointer#autorelease" do
  before :all do
    @libc = Module.new
    @libc.extend Rubinius::FFI::Library
    @libc.ffi_lib Rubinius::FFI::Library::LIBC
    @libc.attach_function :free, [:pointer], :void
  end

  it "starts as enabled" do
    ptr = Rubinius::FFI::MemoryPointer.new :int
    ptr.autorelease?.should == true
  end

  it "can be disabled (and the pointer freed explicitly)" do
    ptr = Rubinius::FFI::MemoryPointer.new :int
    ptr.autorelease = false
    ptr.autorelease?.should == false
    @libc.free ptr
  end

  it "can be disabled and reenabled" do
    ptr = Rubinius::FFI::MemoryPointer.new :int
    ptr.autorelease?.should == true
    ptr.autorelease = false
    ptr.autorelease?.should == false
    ptr.autorelease = true
    ptr.autorelease?.should == true
  end
end
