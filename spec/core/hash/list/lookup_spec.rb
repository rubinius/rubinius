require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::List#lookup" do
    before :each do
      @state = Hash::State.new
      @list = Hash::List.new @state, :a.hash
      @entry = Hash::Item.new :a, 1, @state
      @list.add @entry, :b, :b.hash, 2
    end

    it "returns nil if the entry is not found" do
      @list.lookup(:c, :c.hash).should be_nil
    end

    it "returns the entry if it is found" do
      @list.lookup(:a, :a.hash).should equal(@entry)
    end
  end
end
