require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#isupper" do
  it "returns true if self is between A..Z" do
    a = []
    "MenTaLguY".each_byte do |b|
      a << Rubinius::CType.isupper(b)
    end

    a.should == [true, false, false, true, false, true, false, false, true]
  end
end
