require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Entry.new" do
    before :each do
      @state = Hash::State.new
    end

    it "increments the size of @state" do
      @state.size.should == 0
      entry = Hash::Entry.new :a, 1, @state
      @state.size.should == 1
    end

    it "inserts itself in an empty chain" do
      entry = Hash::Entry.new :a, 1, @state
      @state.head.should equal(entry)
      @state.tail.should equal(entry)
    end

    it "inserts itself at the tail of a non-empty chain" do
      e1 = Hash::Entry.new :a, 1, @state
      e2 = Hash::Entry.new :b, 2, @state
      @state.head.should equal(e1)
      @state.tail.should equal(e2)
    end
  end
end
