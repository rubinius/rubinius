require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Item#delete" do
    before :each do
      @state = Hash::State.new
    end

    it "decrements the size of @state" do
      entry = Hash::Item.new :a, 1, @state
      @state.size.should == 1
      entry.delete(:a, :a.hash)
      @state.size.should == 0
    end

    it "removes itself from a one element insertion chain and sets head and tail of @state to nil" do
      entry = Hash::Item.new :a, 1, @state
      @state.head.should equal(entry)
      @state.tail.should equal(entry)
      entry.delete(:a, :a.hash)
      @state.head.should be_nil
      @state.tail.should be_nil
    end

    it "removes itself from the head of the insertion chain" do
      e1 = Hash::Item.new :a, 1, @state
      e2 = Hash::Item.new :b, 2, @state
      @state.head.should equal(e1)
      @state.tail.should equal(e2)
      e1.delete(:a, :a.hash)
      @state.head.should equal(e2)
      @state.tail.should equal(e2)
    end

    it "removes itself from the tail of the insertion chain" do
      e1 = Hash::Item.new :a, 1, @state
      e2 = Hash::Item.new :b, 2, @state
      @state.head.should equal(e1)
      @state.tail.should equal(e2)
      e2.delete(:b, :b.hash)
      @state.head.should equal(e1)
      @state.tail.should equal(e1)
    end

    it "removes itself from the middle of the insertion chain" do
      e1 = Hash::Item.new :a, 1, @state
      e2 = Hash::Item.new :b, 2, @state
      e3 = Hash::Item.new :c, 3, @state
      @state.head.should equal(e1)
      @state.tail.should equal(e3)
      e2.delete(:b, :b.hash)
      @state.head.should equal(e1)
      @state.tail.should equal(e3)
    end

    it "returns itself if the keys match" do
      entry = Hash::Item.new 1, 2, @state
      @state.should_receive(:match?).with(1, 1).and_return(true)
      entry.delete(1, 0).should equal(entry)
    end

    it "retuns nil if the keys do not match" do
      entry = Hash::Item.new 1, 2, @state
      @state.should_receive(:match?).with(1, 1).and_return(false)
      entry.delete(1, 0).should be_nil
    end
  end
end
