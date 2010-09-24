require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::IdentityMap#size" do
  it "returns 0 if the map is empty" do
    im = Rubinius::IdentityMap.from []
    im.size.should == 0
  end

  it "returns 1 if there is a single entry in the map" do
    im = Rubinius::IdentityMap.from [:a]
    im.size.should == 1
  end

  it "returns the number of entries in the map" do
    im = Rubinius::IdentityMap.from [:a, :a, :b, :c, :b]
    im.size.should == 3
  end
end
