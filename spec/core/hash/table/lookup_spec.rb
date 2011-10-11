require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Table#lookup" do
    before :each do
      @state = Hash::State.new
      @table = Hash::Table.new @state
      @table.insert :a, :a.hash, 1
    end

    it "returns nil if the entry is not found" do
      @table.lookup(:b, :b.hash).should be_nil
    end

    it "returns the entry if it is found" do
      entry = @table.lookup(:a, :a.hash)
      entry.should be_an_instance_of(Hash::Item)
      entry.value.should == 1
    end
  end
end
