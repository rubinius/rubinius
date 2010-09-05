require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::IdentityMap.from" do
  it "imports the contents of the array" do
    im = Rubinius::IdentityMap.from [:a, :b]

    im.include?(:a).should be_true
    im.include?(:b).should be_true
  end

  it "imports an empty array" do
    im = Rubinius::IdentityMap.from [:a, :b], []

    im.include?(:a).should be_true
    im.include?(:b).should be_true
  end

  it "imports an array with one item" do
    im = Rubinius::IdentityMap.from [:a]

    im.include?(:a).should be_true
  end

  it "combines the elements of each array imported" do
    im = Rubinius::IdentityMap.from [:a, :b], [:b, :c]

    im.include?(:a).should be_true
    im.include?(:b).should be_true
    im.include?(:c).should be_true
  end
end
