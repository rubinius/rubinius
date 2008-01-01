require File.dirname(__FILE__) + '/../../spec_helper'

describe "ByteArray#[]=" do
  it "sets the byte at index" do
    s = "01234"
    b = s.data
    0.upto(s.size-1) { |i| b[i] = b[i] + ?A }
    s.should == "qrstu"
  end
end
