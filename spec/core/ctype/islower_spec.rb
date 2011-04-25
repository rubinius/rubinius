require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::CType.islower" do
  it "returns true if self is between a..z" do
    a = []
    "MenTaLguY".each_byte do |b|
      a << Rubinius::CType.islower(b)
    end

    a.should == [false, true, true, false, true, false, true, true, false]
  end
end
