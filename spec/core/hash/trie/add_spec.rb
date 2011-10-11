require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#add" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 0
    end

    it "inserts a List if the entries have the same key hash value" do
      key_hash = 0b1000000

      k1 = mock("Hash key1")
      k1.should_receive(:hash).and_return(key_hash)

      k2 = mock("Hash key2")

      entry = Hash::Item.new k1, 1, @state
      @trie.add entry, k2, key_hash, 2
      @trie.entries[0].should be_an_instance_of(Hash::List)
    end

    it "inserts a Trie if the key hash values collide at this level" do
      k1_hash = 0b1_000001_001000

      k1 = mock("Hash key1")
      k1.should_receive(:hash).twice.and_return(k1_hash)

      k2 = mock("Hash key2")

      entry = Hash::Item.new k1, 1, @state
      @trie.add entry, k2, 0b10_000001_000000, 2
      @trie.entries[0].should be_an_instance_of(Hash::Trie)
    end

    it "inserts two Item instances if the key hash values do not collide" do
      k1_hash = 0b1_000001_001000

      k1 = mock("Hash key1")
      k1.should_receive(:hash).and_return(k1_hash)

      k2 = mock("Hash key2")

      entry = Hash::Item.new k1, 1, @state
      @trie.add entry, k2, 0b10_000010_000000, 2
      @trie.entries[0].should be_an_instance_of(Hash::Item)
      @trie.entries[1].should be_an_instance_of(Hash::Item)
    end
  end
end
