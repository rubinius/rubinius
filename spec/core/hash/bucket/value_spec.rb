require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  ruby_version_is ""..."1.9" do
    describe "Hash::Bucket#value" do
      it "returns the value object" do
        Hash::Bucket.new(1, 3, 2).value.should == 2
      end
    end

    describe "Hash::Bucket#value=" do
      it "sets the value object" do
        b = Hash::Bucket.new 1, 2, 3
        b.value = 3
        b.value.should == 3
      end
    end
  end

  ruby_version_is "1.9" do
    describe "Hash::Bucket#value" do
      it "returns the value object" do
        state = Hash::State.new
        Hash::Bucket.new(1, 3, 2, state).value.should == 2
      end
    end

    describe "Hash::Bucket#value=" do
      it "sets the value object" do
        state = Hash::State.new
        b = Hash::Bucket.new 1, 2, 3, state
        b.value = 3
        b.value.should == 3
      end
    end
  end
end
