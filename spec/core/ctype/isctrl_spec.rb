require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#isctrl" do
  it "returns true if self is an ASCII control character" do
    a = []
    "\tt\fa\r \nt\va\ae\eb\bq".each_byte { |b| a << b.isctrl }
    a.should == [true, false, true, false, true, false, true, false,
                 true, false, true, false, true, false, true, false]
  end
end
