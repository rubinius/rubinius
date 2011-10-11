require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::List#delete" do
    before :each do
      @state = Hash::State.new
      @list = Hash::List.new @state, :a.hash
      @entry = Hash::Item.new :a, 1, @state
      @list.add @entry, :b, :b.hash, 2
    end

    it "returns nil if the entry is not found" do
      @list.delete(:c, :c.hash).should be_nil
    end

    it "removes and returns the entry if it is found" do
      @list.delete(:a, :a.hash).should equal(@entry)
      @list.entries.size.should == 1
    end
  end
end
