require File.expand_path('../../../spec_helper', __FILE__)

describe "FFI::Pointer#size" do
  it "matches 1.size" do
    FFI::Pointer.size.should == 1.size
  end
end
