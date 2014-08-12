require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Item#lookup" do
    before :each do
      @state = Hash::State.new
      @entry = Hash::Item.new 1, 1.hash, 2, @state
    end

    it "returns itself if the keys match" do
      @entry.lookup(1, 1.hash).should equal(@entry)
    end

    it "retuns nil if the keys do not match" do
      @entry.lookup(2, 2.hash).should be_nil
    end
  end
end
