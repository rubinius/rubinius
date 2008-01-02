require File.dirname(__FILE__) + '/../../spec_helper'

describe "CType#tolower" do
  it "returns self in range a..z if self is between A..Z, otherwise return self" do
    a = []
    "MenTaLguY".each_byte { |b| a << b.tolower }
    a.should == [109, 101, 110, 116, 97, 108, 103, 117, 121]
  end
end
