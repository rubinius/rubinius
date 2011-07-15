require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#set_bitmap" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 1
    end

    platform_is :wordsize => 32 do
      it "sets a bit corresponding to the value of a segment of the key hash" do
        [ [@trie, 0b1111100100101001, 0x8000_0000],
          [@trie, 0b1100000100101001, 0x0100_0000],
          [@trie, 0b1000100100101001, 0x0002_0000],
          [@trie, 0b1000000100101001, 0x0001_0000],
          [@trie, 0b0010100100101001, 0x0000_0020],
          [@trie, 0b0010000100101001, 0x0000_0010],
          [@trie, 0b0000100100101001, 0x0000_0002],
          [@trie, 0b0000000100101001, 0x0000_0001],
        ].should be_computed_by(:set_bitmap)
      end
    end

    platform_is :wordsize => 64 do
      it "sets a bit corresponding to the value of a segment of the key hash" do
        [ [@trie, 0b111111000100101001, 0x8000_0000_0000_0000],
          [@trie, 0b101000000100101001, 0x0000_0100_0000_0000],
          [@trie, 0b100001000100101001, 0x0000_0002_0000_0000],
          [@trie, 0b100000000100101001, 0x0000_0001_0000_0000],
          [@trie, 0b000101000100101001, 0x0000_0000_0000_0020],
          [@trie, 0b000100000100101001, 0x0000_0000_0000_0010],
          [@trie, 0b000001000100101001, 0x0000_0000_0000_0002],
          [@trie, 0b000000000100101001, 0x0000_0000_0000_0001],
        ].should be_computed_by(:set_bitmap)
      end
    end
  end
end
