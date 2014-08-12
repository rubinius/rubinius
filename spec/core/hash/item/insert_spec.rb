require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Item#insert" do
    before :each do
      @state = Hash::State.new
      @entry = Hash::Item.new 1, 1.hash, 2, @state
    end

    it "updates the value if the keys match" do
      @entry.insert(1, 1.hash, :value)
      @entry.value.should == :value
    end

    it "returns itself if the keys match" do
      @entry.insert(1, 1.hash, :value).should equal(@entry)
    end

    it "returns nil if the keys do not match" do
      @entry.insert(2, 2.hash, :value).should be_nil
    end

  end
end
