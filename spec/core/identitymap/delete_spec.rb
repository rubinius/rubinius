require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::IdentityMap#delete" do
  before :each do
    @im = Rubinius::IdentityMap.from [:a, :b, :c]
  end

  it "returns false if the map is empty" do
    im = Rubinius::IdentityMap.from []
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
    im = Rubinius::IdentityMap.from [:a]
    im.delete(:a).should be_true
  end

  it "returns true when removing an entry that collides" do
    i1 = mock("item 1")
    i1.should_receive(:hash).twice.and_return(0)
    i2 = mock("item 2")
    i2.should_receive(:hash).twice.and_return(0)
    i3 = mock("item 3")
    i3.should_receive(:hash).twice.and_return(0)

    im = Rubinius::IdentityMap.from [i1, i2, i3]
    im.delete(i1).should be_true
    im.delete(i2).should be_true
    im.delete(i3).should be_true
  end

  it "decrements the size when an entry is deleted" do
    @im.size.should == 3
    @im.delete(:a)
    @im.size.should == 2
  end

  it "does not decrement the size if an entry is not deleted" do
    @im.size.should == 3
    @im.delete(:d)
    @im.size.should == 3
  end
end
