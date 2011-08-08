require File.expand_path('../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  describe "Hash#redistribute" do
    before :each do
      ScratchPad.record []
    end

    it "does not change the contents of the Hash" do
      h = { 'a' => 1, 'b' => 2, 'c' => 3 }
      h.redistribute(h.__entries__)
      h.each { |k, v| ScratchPad << [k, v] }
      ScratchPad.recorded.sort.should == [['a', 1], ['b', 2], ['c', 3]]
    end
  end
end
