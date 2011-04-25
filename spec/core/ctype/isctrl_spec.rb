require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::CType.isctrl" do
  it "returns true if self is an ASCII control character" do
    a = []
    "\tt\fa\r \nt\va\ae\eb\bq".each_byte do |b|
      a << Rubinius::CType.isctrl(b)
    end

    a.should == [true, false, true, false, true, false, true, false,
                 true, false, true, false, true, false, true, false]
  end
end
