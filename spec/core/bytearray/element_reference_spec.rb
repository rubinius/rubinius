require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

describe "ByteArray#[]" do
  before :each do
    ScratchPad.record []
  end

  it "returns the byte at index" do
    str = "A92f"
    b = ByteArraySpecs.new_bytearray str
    (0...str.size).each { |i| ScratchPad << b[i].chr }
    ScratchPad.recorded.should == ["A", "9", "2", "f"]
  end
end
