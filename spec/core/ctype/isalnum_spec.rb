require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#isalnum" do
  it "returns true if self is between A..Z, a..z, 0..9" do
    a = []
    "aAbB9;2-\0005Zt!@#b{$}x9".each_byte { |b| a << b.isalnum }
    a.should == [ true, true, true, true, true, false, true, false, false, true, true,
        true, false, false, false, true, false, false, false, true, true]
  end
end
