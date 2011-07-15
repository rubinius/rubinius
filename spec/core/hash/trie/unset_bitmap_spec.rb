require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Trie#unset_bitmap" do
    before :each do
      @state = Hash::State.new
      @trie = Hash::Trie.new @state, 1
    end

    platform_is :wordsize => 32 do
      it "unsets a bit corresponding to the value of a segment of the key hash" do
        @trie.bmp = 0x8103_0033

        [ [@trie, 0b1111100100101001, 0x0103_0033],
          [@trie, 0b1100000100101001, 0x8003_0033],
          [@trie, 0b1000100100101001, 0x8101_0033],
          [@trie, 0b1000000100101001, 0x8102_0033],
          [@trie, 0b0010100100101001, 0x8103_0013],
          [@trie, 0b0010000100101001, 0x8103_0023],
          [@trie, 0b0000100100101001, 0x8103_0031],
          [@trie, 0b0000000100101001, 0x8103_0032],
        ].should be_computed_by(:unset_bitmap)
      end
    end

    platform_is :wordsize => 64 do
      it "unsets a bit corresponding to the value of a segment of the key hash" do
        @trie.bmp = 0x8000_0103_0000_0033

        [ [@trie, 0b111111000100101001, 0x0000_0103_0000_0033],
          [@trie, 0b101000000100101001, 0x8000_0003_0000_0033],
          [@trie, 0b100001000100101001, 0x8000_0101_0000_0033],
          [@trie, 0b100000000100101001, 0x8000_0102_0000_0033],
          [@trie, 0b000101000100101001, 0x8000_0103_0000_0013],
          [@trie, 0b000100000100101001, 0x8000_0103_0000_0023],
          [@trie, 0b000001000100101001, 0x8000_0103_0000_0031],
          [@trie, 0b000000000100101001, 0x8000_0103_0000_0032],
        ].should be_computed_by(:unset_bitmap)
      end
    end
  end
end
