require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#delete" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 0
      @entry = Hash::Item.new :a, 1, @state
      @trie.add @entry, :b, :b.hash, 2
    end

    it "returns nil if the entry is not found" do
      @trie.delete(:c, :c.hash).should be_nil
    end

    it "returns the entry if it is found" do
      @trie.delete(:a, :a.hash).should equal(@entry)
    end
  end
end
