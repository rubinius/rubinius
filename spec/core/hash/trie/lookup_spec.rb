require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#lookup" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 0
    end

    it "returns nil if the entry is not found" do
      @trie.lookup(:a, :a.hash).should be_nil
    end

    it "returns the entry if it is found" do
      entry = Hash::Item.new :a, 1, @state
      @trie.add entry, :b, :b.hash, 2
      @trie.lookup(:a, :a.hash).should equal(entry)
    end
  end
end
