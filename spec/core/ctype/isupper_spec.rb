require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#isupper" do
  it "returns true if self is between A..Z" do
    a = []
    "MenTaLguY".each_byte { |b| a << b.isupper }
    a.should == [true, false, false, true, false, true, false, false, true]
  end
end
