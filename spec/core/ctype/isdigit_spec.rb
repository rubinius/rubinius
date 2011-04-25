require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#isdigit" do
  it "returns true if self is between 0..9" do
    a = []
    "0a1b2C3;4:5d6=7+8p9".each_byte do |b|
      a << Rubinius::CType.isdigit(b)
    end

    a.should == [ true, false, true, false, true, false, true, false, true, false, true, 
        false, true, false, true, false, true, false, true]
  end
end
