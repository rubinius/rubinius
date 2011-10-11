require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Table#insert" do
    before :each do
      @state = Hash::State.new
      @table = Hash::Table.new @state
    end

    it "inserts an Item if there is no entry at the index" do
      @table.insert(:a, 0, 1)
      @table.entries[0].should be_an_instance_of(Hash::Item)
    end

    it "inserts a Trie if there is an Item at the index" do
      k1 = mock("Hash key1")
      k1_hash = 0b01000000
      k1.should_receive(:hash).and_return(k1_hash)

      k2 = mock("Hash key2")

      @table.insert(k1, k1_hash, 1)
      @table.insert(k2, 0b10000000, 2)

      @table.entries[0].should be_an_instance_of(Hash::Trie)
    end

    it "causes the entry's value to be updated" do
      @table.insert(:a, 0, 3)
      @table.entries[0].value.should == 3

      @table.insert(:a, 0, 5)
      @table.entries[0].value.should == 5
    end
  end
end
