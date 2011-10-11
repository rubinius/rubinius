require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#item_index" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 1
    end

    it "returns nil if there is no entry set for the key hash value" do
      @trie.item_index(0).should be_nil
    end

    platform_is :wordsize => 32 do
      it "returns the index of the entry for the key hash value" do
        @trie.bmp = 0x8103_0033

        [ [@trie, 0b1111100100101001, 7],
          [@trie, 0b1100000100101001, 6],
          [@trie, 0b1000100100101001, 5],
          [@trie, 0b1000000100101001, 4],
          [@trie, 0b0010100100101001, 3],
          [@trie, 0b0010000100101001, 2],
          [@trie, 0b0000100100101001, 1],
          [@trie, 0b0000000100101001, 0],
        ].should be_computed_by(:item_index)
      end
    end

    platform_is :wordsize => 64 do
      it "returns the index of the entry for the key hash value" do
        @trie.bmp = 0x8000_0103_0000_0033

        [ [@trie, 0b111111000100101001, 7],
          [@trie, 0b101000000100101001, 6],
          [@trie, 0b100001000100101001, 5],
          [@trie, 0b100000000100101001, 4],
          [@trie, 0b000101000100101001, 3],
          [@trie, 0b000100000100101001, 2],
          [@trie, 0b000001000100101001, 1],
          [@trie, 0b000000000100101001, 0],
        ].should be_computed_by(:item_index)
      end
    end
  end
end
