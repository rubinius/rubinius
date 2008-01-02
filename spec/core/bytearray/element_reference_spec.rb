require File.dirname(__FILE__) + '/../../spec_helper'

describe "ByteArray#[]" do
  it "returns the byte at index" do
    s= "A92f"
    b = s.data
    a = []
    0.upto(s.size-1) { |i| a << b[i].chr }
    a.should == ["A", "9", "2", "f"]
  end
end
