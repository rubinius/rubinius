require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Entry#value" do
  it "returns the value object" do
    Hash::Entry.new(1, 3, 2).value.should == 2
  end
end

describe "Hash::Entry#value=" do
  it "sets the value object" do
    b = Hash::Entry.new 1, 2, 3
    b.value = 3
    b.value.should == 3
  end
end
