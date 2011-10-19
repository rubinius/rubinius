require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Item#insert" do
    before :each do
      @state = Hash::State.new
      @entry = Hash::Item.new 1, 2, @state
    end

    it "updates the value if the keys match" do
      @state.should_receive(:match?).with(1, 1).and_return(true)
      @entry.insert(1, 0, :value)
      @entry.value.should == :value
    end

    it "returns itself if the keys match" do
      @state.should_receive(:match?).with(1, 1).and_return(true)
      @entry.insert(1, 0, :value).should equal(@entry)
    end

    it "returns nil if the keys do not match" do
      @state.should_receive(:match?).with(1, 1).and_return(false)
      @entry.insert(1, 0, :value).should be_nil
    end

  end
end
