require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::IdentityMap#include?" do
  before :each do
    @im = Array::IdentityMap.new [:a, :b, :c]
  end

  it "returns false if the map is empty" do
    im = Array::IdentityMap.new []
    im.include?(:a).should be_false
  end

  it "returns false if the item is not in the map" do
    @im.include?(:d).should be_false
  end

  it "returns false if the item has been deleted" do
    @im.delete(:b).should be_true
    @im.include?(:b).should be_false
  end

  it "returns true if the item is in the map" do
    @im.include?(:a).should be_true
  end

  it "returns true when the item is the only entry in the map" do
    im = Array::IdentityMap.new [:a]
    im.include?(:a).should be_true
  end

  it "returns true when included entries collide" do
    i1 = mock("item 1")
    i1.should_receive(:hash).twice.and_return(0)
    i2 = mock("item 2")
    i2.should_receive(:hash).twice.and_return(0)
    i3 = mock("item 3")
    i3.should_receive(:hash).twice.and_return(0)

    im = Array::IdentityMap.new [i1, i2, i3]
    im.include?(i1).should be_true
    im.include?(i2).should be_true
    im.include?(i3).should be_true
  end

  it "returns true when a colliding entry has been deleted" do
    i1 = mock("item 1")
    i1.should_receive(:hash).any_number_of_times.and_return(0)
    i2 = mock("item 2")
    i2.should_receive(:hash).any_number_of_times.and_return(0)

    im = Array::IdentityMap.new [i1, i2]
    im.include?(i1).should be_true
    im.include?(i2).should be_true

    im.delete(i1)
    im.include?(i1).should be_false
    im.include?(i2).should be_true
  end
end
