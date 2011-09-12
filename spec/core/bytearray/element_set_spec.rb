require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

describe "ByteArray#[]=" do
  before :each do
    ScratchPad.record []
  end

  it "sets the byte at index" do
    str = "01234"
    b = ByteArraySpecs.new_bytearray str
    (0...str.size).each { |i| b[i] = b[i] + 65 }

    "qrstu".each_byte.with_index { |x, i| b[i].should == x }
  end
end
