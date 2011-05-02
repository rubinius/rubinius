require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::CType.toupper" do
  it "returns a value in the range A..Z if self is between a..z, otherwise return self" do
    a = []
    "MenTaLguY".each_byte do |b|
      a << Rubinius::CType.toupper(b)
    end

    a.should == [77, 69, 78, 84, 65, 76, 71, 85, 89]
  end
end

describe "CType#toupper!" do
  it "unconditionally subtracts 32 from self (use only if previously testing isupper or islower)" do
    a = []
    "MenTaLguY".each_byte do |b|
      a << Rubinius::CType.toupper!(b)
    end

    a.should == [45, 69, 78, 52, 65, 44, 71, 85, 57]
  end
end
