require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::IdentityMap#delete" do
  before :each do
    @im = Array::IdentityMap.new [:a, :b, :c]
  end

  it "returns false if the map is empty" do
    im = Array::IdentityMap.new []
    im.delete(:a).should be_false
  end

  it "returns false if the item is not in the map" do
    @im.delete(:d).should be_false
  end

  it "returns true if the item is in the map" do
    @im.delete(:a).should be_true
  end

  it "returns true for each item in the map" do
    @im.delete(:a).should be_true
    @im.delete(:b).should be_true
    @im.delete(:c).should be_true
  end

  it "returns true when the item is the only entry in the map" do
    im = Array::IdentityMap.new [:a]
    im.delete(:a).should be_true
  end

  it "returns true when removing an entry that collides" do
    i1 = mock("item 1")
    i1.should_receive(:hash).twice.and_return(0)
    i2 = mock("item 2")
    i2.should_receive(:hash).twice.and_return(0)
    i3 = mock("item 3")
    i3.should_receive(:hash).twice.and_return(0)

    im = Array::IdentityMap.new [i1, i2, i3]
    im.delete(i1).should be_true
    im.delete(i2).should be_true
    im.delete(i3).should be_true
  end
end
