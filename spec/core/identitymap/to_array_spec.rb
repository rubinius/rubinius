require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::IdentityMap#to_array" do
  it "returns an empty Rubinius if the map is empty" do
    im = Rubinius::IdentityMap.from []
    im.to_array.should == []
  end

  it "returns an Rubinius of the entries in the map" do
    im = Rubinius::IdentityMap.from [:a, :b, :c, :b, :a]
    im.to_array.should == [:a, :b, :c]
  end
end
