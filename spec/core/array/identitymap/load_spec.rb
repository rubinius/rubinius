require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::IdentityMap#load" do
  it "imports the contents of the array" do
    im = Array::IdentityMap.new []
    im.load [:a, :b]

    im.include?(:a).should be_true
    im.include?(:b).should be_true
  end

  it "imports an empty array" do
    im = Array::IdentityMap.new [:a, :b]
    im.load []

    im.include?(:a).should be_true
    im.include?(:b).should be_true
  end

  it "imports an array with one item" do
    im = Array::IdentityMap.new []
    im.load [:a]

    im.include?(:a).should be_true
  end

  it "combines the elements of each array imported" do
    im = Array::IdentityMap.new [:a, :b]
    im.load [:b, :c]

    im.include?(:a).should be_true
    im.include?(:b).should be_true
    im.include?(:c).should be_true
  end
end
