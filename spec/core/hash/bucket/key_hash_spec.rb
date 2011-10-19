require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  ruby_version_is ""..."1.9" do
    describe "Hash::Bucket#key_hash" do
      it "returns the key hash object" do
        Hash::Bucket.new(1, 2, 33).key_hash.should == 2
      end
    end

    describe "Hash::Bucket#key_hash=" do
      it "sets the value object" do
        b = Hash::Bucket.new 1, 2, 3
        b.key_hash = 33
        b.key_hash.should == 33
      end
    end
  end

  ruby_version_is "1.9" do
    describe "Hash::Bucket#key_hash" do
      it "returns the key hash object" do
        state = Hash::State.new
        Hash::Bucket.new(1, 2, 33, state).key_hash.should == 2
      end
    end

    describe "Hash::Bucket#key_hash=" do
      it "sets the value object" do
        state = Hash::State.new
        b = Hash::Bucket.new 1, 2, 3, state
        b.key_hash = 33
        b.key_hash.should == 33
      end
    end
  end
end
