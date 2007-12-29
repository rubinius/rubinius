require File.dirname(__FILE__) + '/../../../spec_helper'

describe "CType#toupper" do
  it "returns a value in the range A..Z if self is between a..z, otherwise return self" do
    a = []
    "MenTaLguY".each_byte { |b| a << b.toupper }
    a.should == [77, 69, 78, 84, 65, 76, 71, 85, 89]
  end
end
