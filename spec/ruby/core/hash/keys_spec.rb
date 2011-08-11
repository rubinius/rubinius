require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Hash#keys" do

  ruby_version_is ""..."1.9" do
    it "returns an array populated with keys" do
      new_hash.keys.should == []
      new_hash.keys.should be_kind_of(Array)
      new_hash(5).keys.should == []
      new_hash { 5 }.keys.should == []
      new_hash(1 => 2, 2 => 4, 4 => 8).keys.sort.should == [1, 2, 4]
      new_hash(1 => 2, 2 => 4, 4 => 8).keys.should be_kind_of(Array)
      new_hash(nil => nil).keys.should == [nil]
    end
  end

  ruby_version_is "1.9" do
    it "returns an empty Array when empty" do
      new_hash.keys.should == []
    end

    it "returns an array with the keys in the order they were inserted" do
      # Creating the hash from an Array of values is necessary so the spec is
      # correct when the "system" Hash doesn't support insertion ordering
      new_hash(1, 2, 4, 8, 2, 4).keys.should == [1, 4, 2]
    end

    it "does not use the default value when empty" do
      new_hash(5).keys.should == []
    end

    it "does not use the default proc when empty" do
      h = new_hash { 5 }
      h.keys.should == []
    end
  end
end
