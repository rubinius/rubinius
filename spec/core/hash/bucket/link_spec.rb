require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  ruby_version_is ""..."1.9" do
    describe "Hash::Bucket#link" do
      it "returns nil if there is no chained entry" do
        Hash::Bucket.new(1, 3, 2).link.should be_nil
      end

      it "returns the next entry" do
        a = Hash::Bucket.new 1, 2, 3
        b = Hash::Bucket.new 4, 5, 6
        a.link = b
        a.link.should == b
      end
    end
  end

  ruby_version_is "1.9" do
    describe "Hash::Bucket#link" do
      it "returns nil if there is no chained entry" do
        state = Hash::State.new
        Hash::Bucket.new(1, 3, 2, state).link.should be_nil
      end

      it "returns the next entry" do
        state = Hash::State.new
        a = Hash::Bucket.new 1, 2, 3, state
        b = Hash::Bucket.new 4, 5, 6, state
        a.link = b
        a.link.should == b
      end
    end
  end
end
