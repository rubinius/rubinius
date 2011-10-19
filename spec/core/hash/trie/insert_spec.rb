require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#insert" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 0
      @trie.entries = Rubinius::Tuple[nil]
    end

    it "inserts an Item if there is no entry at the index" do
      @trie.insert(:a, 0, 1)
      entry = @trie.entries[0]

      entry.should be_an_instance_of(Hash::Item)
      entry.key.should equal(:a)
    end

    it "inserts a Trie if there is an entry at the index" do
      k1 = mock("Hash key1")
      k1.should_receive(:hash).and_return(0)

      @trie.insert k1, 0, 1
      @trie.insert :b, 0, 2

      @trie.entries[0].should be_an_instance_of(Hash::Trie)
    end
  end
end
