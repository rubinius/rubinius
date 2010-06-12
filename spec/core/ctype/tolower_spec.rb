require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#tolower" do
  it "returns self in range a..z if self is between A..Z, otherwise return self" do
    a = []
    "MenTaLguY".each_byte { |b| a << b.tolower }
    a.should == [109, 101, 110, 116, 97, 108, 103, 117, 121]
  end
end

describe "CType#tolower!" do
  it "unconditionally adds 32 to self (use only if previously testing isupper or islower)" do
    a = []
    "MenTaLguY".each_byte { |b| a << b.tolower! }
    a.should == [109, 133, 142, 116, 129, 108, 135, 149, 121]
  end
end
