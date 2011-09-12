require File.expand_path('../../../spec_helper', __FILE__)

describe "CharArray#[]=" do
  it "sets the byte at index" do
    s = "01234"
    b = s.data
    0.upto(s.size-1) { |i| b[i] = b[i] + 65 }
    s.should == "qrstu"
  end
end
