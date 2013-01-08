require File.expand_path('../../../spec_helper', __FILE__)
require 'rubinius/ffi'

describe "FFI::Pointer#size" do
  it "matches 1.size" do
    FFI::Pointer.size.should == 1.size
  end

  platform_is :wordsize => 32 do
    it "returns its address for large values" do
      FFI::Pointer.new(0xdeadbeef).address.should == 0xdeadbeef
    end
  end

  platform_is :wordsize => 64 do
    it "returns its address for large values" do
      FFI::Pointer.new(0x8000000000000000).address.should == 0x8000000000000000
    end
  end
end
