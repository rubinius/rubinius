require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::List#insert" do
    before :each do
      @state = Hash::State.new
      @list = Hash::List.new @state, :a.hash
      @entry = Hash::Item.new :a, 1, @state
      @list.add @entry, :b, :b.hash, 2
    end

    it "returns the entry if it is found" do
      @list.insert(:a, :a.hash, 3).should equal(@entry)
    end

    it "causes the entry's value to be updated" do
      @list.insert(:a, :a.hash, 3)
      @entry.value.should == 3
    end

    it "inserts a new entry if the entry is not found" do
      @list.insert(:c, :c.hash, 3).should be_an_instance_of(Hash::Item)
      @list.entries.size.should == 3
    end
  end
end
