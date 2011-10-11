require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Item#lookup" do
    before :each do
      @state = Hash::State.new
      @entry = Hash::Item.new 1, 2, @state
    end

    it "returns itself if the keys match" do
      @state.should_receive(:match?).with(1, 1).and_return(true)
      @entry.lookup(1, 0).should equal(@entry)
    end

    it "retuns nil if the keys do not match" do
      @state.should_receive(:match?).with(1, 1).and_return(false)
      @entry.lookup(1, 0).should be_nil
    end
  end
end
