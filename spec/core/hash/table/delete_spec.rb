require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Table#delete" do
    before :each do
      @state = Hash::State.new
      @table = Hash::Table.new @state
      @table.insert :a, :a.hash, 1
      @entry = @table.entries[@table.entry_index(:a.hash)]
    end

    it "returns nil if the entry is not found" do
      @table.delete(:b, :b.hash).should be_nil
    end

    it "returns the entry if it is found" do
      @table.delete(:a, :a.hash).should equal(@entry)
    end

    it "sets the index to nil if the Entry is deleted" do
      @table.entries[@table.entry_index(:a.hash)].should equal(@entry)
      @table.delete(:a, :a.hash)
      @table.entries[@table.entry_index(:a.hash)].should be_nil
    end
  end
end
