require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::IdentityMap#to_array" do
  it "returns an empty Array if the map is empty" do
    im = Array::IdentityMap.new []
    im.to_array.should == []
  end

  it "returns an Array of the entries in the map" do
    im = Array::IdentityMap.new [:a, :b, :c, :b, :a]
    im.to_array.should == [:a, :b, :c]
  end

  it "replaces the contents of the passed array" do
    im = Array::IdentityMap.new [:a, :b, :c, :b, :a]
    array = [1, 2, 3, 4, 5]
    ary = im.to_array(array)
    array.should equal(ary)
    array.should == [:a, :b, :c]
  end
end
