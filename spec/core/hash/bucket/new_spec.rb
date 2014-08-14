require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  describe "Hash::Bucket.new" do
    it "creates a new Bucket" do
      b = Hash::Bucket.new(1, 2, 3)
      b.key.should == 1
      b.key_hash.should == 2
      b.value.should == 3
      b.link.should be_nil
    end
  end
end
