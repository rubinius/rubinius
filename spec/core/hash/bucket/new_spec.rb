require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  ruby_version_is ""..."1.9" do
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

  ruby_version_is "1.9" do
    describe "Hash::Bucket.new" do
      it "creates a new Bucket" do
        state = Hash::State.new
        b = Hash::Bucket.new(1, 2, 3, state)
        b.key.should == 1
        b.key_hash.should == 2
        b.value.should == 3
        b.link.should be_nil
      end
    end
  end
end
